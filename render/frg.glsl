#version 430 core

out vec4 Color;
in vec2 tex_coord;

void main()
{
	Color = vec4(0.3, tex_coord.x, tex_coord.t, 1.0);
}