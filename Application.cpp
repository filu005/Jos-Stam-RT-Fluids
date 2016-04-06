#include "Application.hpp"

Application::Application()
{
	_paintables.push_back(&_sim.grid);

	_painter.set_camera(camera);
}

void Application::paint(void)
{
	for(auto & p : _paintables)
		p->paint(_painter);
}


void Application::tick(void)
{
	_sim.run();
}

void Application::add_density(int idx_x, int idx_y)
{
	_sim.add_density(idx_x, idx_y);
}