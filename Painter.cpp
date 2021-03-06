// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Grid.hpp"
#include "Camera.hpp"
#include "Painter.hpp"

Painter::Painter() : grid_shader("./shaders/grid_2d.vert", "./shaders/grid_2d.frag")
{

}


void Painter::paint(Grid const & grid)
{
	grid_shader.Use();

	// Create transformations
	glm::mat4 view;
	glm::mat4 model;
	glm::mat4 projection;
	assert(camera_ref != nullptr);
	auto const camera = *camera_ref;

	view = camera.GetViewMatrix();
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	projection = glm::perspective(camera.Zoom, c::aspectRatio, 0.1f, 1000.0f);

	// Get their uniform location
	GLint viewLoc = glGetUniformLocation(grid_shader.Program, "view");
	GLint modelLoc = glGetUniformLocation(grid_shader.Program, "model");
	GLint projLoc = glGetUniformLocation(grid_shader.Program, "projection");
	// Pass them to the shaders
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grid.get_density_texture());
	glUniform1i(glGetUniformLocation(grid_shader.Program, "density_texture"), 0);

	auto const & VAO = grid.getVAO();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6 * 2, grid.bin_count);
	glBindVertexArray(0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
