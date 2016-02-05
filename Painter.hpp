#pragma once
#include "Shader.hpp"

class Grid;

class Camera;

class Painter
{
public:
	Painter();

	void paint(Grid const & grid);

	void set_camera(Camera const & camera) { camera_ref = &camera; }

private:
	Shader grid_shader;

	Camera const * camera_ref;// Reference from Application class
};
