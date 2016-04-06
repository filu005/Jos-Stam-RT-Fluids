#include <cmath>
#include <amp.h>
#include "Simulation.hpp"

Simulation::Simulation()
{

}

void Simulation::run()
{
	velocity_step(grid.u, grid.v, grid.u_prev, grid.v_prev);
	density_step(grid.d, grid.d_prev, grid.u, grid.v);
	grid.update_buffers();
}

void Simulation::add_density(int idx_x, int idx_y)
{
	if(idx_x >= 0 && idx_x < c::K && idx_y >= 0 && idx_y < c::L)
		grid.d[grid::idx(idx_x, idx_y)] += 40000.0f * c::dt;
}

void Simulation::add_source(std::array<float, c::KL> & x, std::array<float, c::KL> & s)
{
	//using namespace concurrency;

	//float a = c::dt;
	//array_view<float, 1> p_x(c::KL, x.data());
	//array_view<const float, 1> p_s(c::KL, s.data());

	//parallel_for_each(p_x.extent, [=](index<1> idx) restrict(amp)
	//{
	//	p_x[idx] += a * p_s[idx];
	//});

	for(int i = 0; i < c::KL; ++i)
		x[i] += c::dt * s[i];
}

void Simulation::diffuse(int b, std::array<float, c::KL> & x, std::array<float, c::KL> & x0)
{
	using namespace c;
	using grid::idx;
	auto const a = dt * diffusion * static_cast<float>(KL);
	auto const factor = 1.0f / (1.0f + 4.0f * a);

	for(int k = 0; k < 10; k++)
	{
		for(int i = 1; i < (K - 1); i++)
		{
			for(int j = 1; j < (L - 1); j++)
			{
				x[idx(i, j)] = (x0[idx(i, j)] + a*(x[idx(i - 1, j)] + x[idx(i + 1, j)] +
												   x[idx(i, j - 1)] + x[idx(i, j + 1)])) * factor;
			}
		}
		set_boundary(b, x);
	}
}

void Simulation::advect(int b, std::array<float, c::KL> & d, std::array<float, c::KL> & d0, std::array<float, c::KL> & u, std::array<float, c::KL> & v)
{
	using namespace c;
	using grid::idx;

	auto const dt0 = dt*static_cast<float>(K);
	for(int i = 1; i < (K - 1); i++)
	{
		for(int j = 1; j < (L - 1); j++)
		{
			int i0, j0, i1, j1;
			float x, y, s0, t0, s1, t1;

			x = i - dt0*u[idx(i, j)]; y = j - dt0*v[idx(i, j)];
			if(x < 0.5f) x = 0.5f;
			if(x > static_cast<float>(K) + 0.5f) x = static_cast<float>(K) + 0.5f;
			i0 = static_cast<int>(x);
			i1 = i0 + 1;
			if(y<0.5f) y = 0.5f;
			if(y > static_cast<float>(K) + 0.5f) y = static_cast<float>(K) + 0.5f;
			j0 = static_cast<int>(y);
			j1 = j0 + 1;
			s1 = x - i0; s0 = 1.0f - s1; t1 = y - j0; t0 = 1.0f - t1;
			d[idx(i, j)] = s0*(t0*d0[idx(i0, j0)] + t1*d0[idx(i0, j1)]) +
				s1*(t0*d0[idx(i1, j0)] + t1*d0[idx(i1, j1)]);
		}
	}
	set_boundary(b, d);
}

void Simulation::density_step(std::array<float, c::KL> & x, std::array<float, c::KL> & x0, std::array<float, c::KL> & u, std::array<float, c::KL> & v)
{
	using namespace c;

	add_source(x, x0);
	diffuse(0, x0, x);
	advect(0, x, x0, u, v);
}

void Simulation::velocity_step(std::array<float, c::KL> & u, std::array<float, c::KL> & v, std::array<float, c::KL> & u0, std::array<float, c::KL> & v0)
{
	add_source(u, u0); add_source(v, v0);
	diffuse(1, u0, u);
	diffuse(2, v0, v);
	project(u0, v0, u, v);
	advect(1, u, u0, u0, v0); advect(2, v, v0, u0, v0);
	project(u, v, u0, v0);
}

void Simulation::project(std::array<float, c::KL> & u, std::array<float, c::KL> & v, std::array<float, c::KL> & p, std::array<float, c::KL> & div)
{
	using namespace c;
	using grid::idx;

	for(int i = 1; i < (K - 1); i++)
	{
		for(int j = 1; j < (L - 1); j++)
		{
			div[idx(i, j)] = -0.5f*dx*(u[idx(i + 1, j)] - u[idx(i - 1, j)] +
				v[idx(i, j + 1)] - v[idx(i, j - 1)]);
			p[idx(i, j)] = 0.0f;
		}
	}

	set_boundary(0, div); set_boundary(0, p);

	for(int k = 0; k < 10; k++)
	{
		for(int i = 1; i < (K - 1); i++)
		{
			for(int j = 1; j < (L - 1); j++)
			{
				p[idx(i, j)] = (div[idx(i, j)] + p[idx(i - 1, j)] + p[idx(i + 1, j)] +
					p[idx(i, j - 1)] + p[idx(i, j + 1)]) / 4.0f;
			}
		}
		set_boundary(0, p);
	}
	for(int i = 1; i < (K - 1); i++)
	{
		for(int j = 1; j < (L - 1); j++)
		{
			u[idx(i, j)] -= 0.5f*(p[idx(i + 1, j)] - p[idx(i - 1, j)]) / dx;
			v[idx(i, j)] -= 0.5f*(p[idx(i, j + 1)] - p[idx(i, j - 1)]) / dx;
		}
	}
	set_boundary(1, u); set_boundary(2, v);
}

void Simulation::set_boundary(int b, std::array<float, c::KL> & x)
{
	return;

	using namespace c;
	using grid::idx;
	auto const N = K - 2;

	for(int i = 1; i < (N + 1); i++)
	{
		x[idx(0, i)] = (b == 1 ? -(x[idx(1, i)]) : x[idx(1, i)]);
		x[idx(N + 1, i)] = (b == 1 ? -(x[idx(N, i)]) : x[idx(N, i)]);
		x[idx(i, 0)] = (b == 2 ? -(x[idx(i, 1)]) : x[idx(i, 1)]);
		x[idx(i, N + 1)] = (b == 2 ? -(x[idx(i, N)]) : x[idx(i, N)]);
	}
	x[idx(0, 0)] = 0.5f*(x[idx(1, 0)] + x[idx(0, 1)]);
	x[idx(0, N + 1)] = 0.5f*(x[idx(1, N + 1)] + x[idx(0, N)]);
	x[idx(N + 1, 0)] = 0.5f*(x[idx(N, 0)] + x[idx(N + 1, 1)]);
	x[idx(N + 1, N + 1)] = 0.5f*(x[idx(N + 1, N)] + x[idx(N, N + 1)]);
}