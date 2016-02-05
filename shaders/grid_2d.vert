#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 offset;

out vec3 Position;
out vec3 outColor;

// uniform vec3 color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position + offset, 0.0, 1.0);
    Position = vec3(model * vec4(position, 0.0, 1.0));
    outColor = vec3(1.0, 0.4, 0.2);
}