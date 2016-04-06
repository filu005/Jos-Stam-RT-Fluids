#include "Painter.hpp"
#include "Grid.hpp"


GLfloat const Grid::square_vertices[6 * 2] =
{
	// First triangle
	c::dx, c::dy,
	c::dx, -c::dy,
	-c::dx, c::dy,
	// Second triangle
	c::dx, -c::dy,
	-c::dx, -c::dy,
	-c::dx, c::dy
};

Grid::Grid()
{
	setup_buffers();
	clear_grid();
}

void Grid::paint(Painter& p) const
{
	p.paint(*this);
}

void Grid::setup_buffers(void)
{
	auto index = 0u;

	for(float y = c::ymin; y < c::ymax; y += c::dy)
		for(float x = c::xmin; x < c::xmax; x += c::dx)
			translations[index++] = glm::vec2(x, y);// post-increment

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->square_vertices), &this->square_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Store instance data in an array buffer
	glGenBuffers(1, &this->instance_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->instance_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * c::KL, &this->translations[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Vertex Positions
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);

	// Also set instance data
	glBindBuffer(GL_ARRAY_BUFFER, this->instance_VBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glVertexAttribDivisor(1, 1); // Tell OpenGL this is an instanced vertex attribute.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenTextures(1, &density_texture);

	GLenum target = GL_TEXTURE_2D;
	GLenum filter = GL_LINEAR;
	GLenum address = GL_CLAMP_TO_EDGE;

	glBindTexture(target, density_texture);

	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);

	glTexParameteri(target, GL_TEXTURE_WRAP_S, address);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, address);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	glTexImage2D(target,
		0,
		GL_RGBA,
		c::K,
		c::L,
		0,
		GL_RGBA,
		GL_FLOAT,
		NULL);

	glBindTexture(target, 0);

	glBindVertexArray(0);
}

void Grid::update_buffers()
{
	std::array<GLfloat, c::KL * 4> rgba_data;

	for(int i = 0; i < c::KL; ++i)
	{
		//rgba_data[i * 4] = normal.x;
		//rgba_data[i * 4 + 1] = normal.y;
		//rgba_data[i * 4 + 2] = normal.z;
		rgba_data[i * 4 + 3] = d[i];
	}


	// upload to DistanceField::volume_texture
	glBindTexture(GL_TEXTURE_2D, this->density_texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, c::K, c::L, GL_RGBA, GL_FLOAT, &rgba_data[0]);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Grid::clear_grid()
{
	u.fill(0.0f);
	u_prev.fill(0.0f);
	v.fill(0.0f);
	v_prev.fill(0.0f);
	d.fill(0.0f);
	d_prev.fill(0.0f);
}