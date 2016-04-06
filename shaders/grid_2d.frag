#version 330 core
uniform sampler2D density_texture;
in vec4 Position;
in vec3 outColor;

out vec4 color;

void main()
{
	vec2 projected_tex_coords = vec2(((Position.x / Position.w) + 1.0f ) / 2.0f,
									  ((Position.y / Position.w) + 1.0f ) / 2.0f );

	float density = texture(density_texture, projected_tex_coords).a;

    color = vec4(vec3(density), 1.0);
}
