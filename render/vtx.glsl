#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

layout (std140, binding = 2) uniform MainView
{
	mat4 view;
	mat4 projection;
	mat4 projview;
};

out vec2 tex_coord;

void main()
{
	gl_Position = projview * vec4(pos, 1.0);
	tex_coord = uv;
}