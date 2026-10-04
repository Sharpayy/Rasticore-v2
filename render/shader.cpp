#include "shader.h"

ShaderNoType::ShaderNoType(GLuint id)
{
	this->id = id;
}

SimpleProgram::SimpleProgram()
{
	id = glCreateProgram();
}

void SimpleProgram::AttachShader(ShaderNoType shdr)
{
	glAttachShader(id, shdr.id);
}

void SimpleProgram::Compile()
{
	glLinkProgram(id);
}

void SimpleProgram::Use()
{
	glUseProgram(id);
}
