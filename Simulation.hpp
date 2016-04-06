#pragma once

#include "Grid.hpp"


class Simulation
{
public:
	Simulation();

	void run();

	void add_density(int idx_x, int idx_y);
	void add_source(std::array<float, c::KL> & x, std::array<float, c::KL> & s);
	void diffuse(int b, std::array<float, c::KL> & x, std::array<float, c::KL> & x0);
	void advect(int b, std::array<float, c::KL> & d, std::array<float, c::KL> & d0, std::array<float, c::KL> & u, std::array<float, c::KL> & v);
	void density_step(std::array<float, c::KL> & d, std::array<float, c::KL> & d0, std::array<float, c::KL> & u, std::array<float, c::KL> & v);
	void velocity_step(std::array<float, c::KL> & u, std::array<float, c::KL> & v, std::array<float, c::KL> & u0, std::array<float, c::KL> & v0);
	void project(std::array<float, c::KL> & u, std::array<float, c::KL> & v, std::array<float, c::KL> & p, std::array<float, c::KL> & div);
	void set_boundary(int b, std::array<float, c::KL> & x);

	// main components and also Paintables
	Grid grid;

private:

};