#pragma once
#include <array>

#include "constants.hpp"
#include "Paintable.hpp"

namespace grid
{
	static int idx(int const x, int const y)
	{
		return (y*c::K + x);
	}
}

class Grid : public Paintable
{
public:
	friend class Simulation; // do macania tablic u, v, d

	Grid();

	void paint(Painter& p) const override final;
	void setup_buffers() override final;

	GLuint const get_density_texture() const { return density_texture; }
	void update_buffers();
	void clear_grid();

	GLsizei const bin_count = c::KL;

private:
	// Hot stuff
	std::array<float, c::KL> u, u_prev;
	std::array<float, c::KL> v, v_prev;
	std::array<float, c::KL> d, d_prev;

	// Geometry, instance offset array
	GLfloat const static square_vertices[6 * 2];
	std::array<glm::vec2, c::KL> translations;

	// OpenGL
	GLuint instance_VBO;
	GLuint density_texture;
};
