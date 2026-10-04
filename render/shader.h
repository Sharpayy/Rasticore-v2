#pragma once
#include <GL/glew.h>
#include <string>

class ShaderNoType
{
public:
	GLuint id;
	ShaderNoType(GLuint id);
};

template <GLenum TYPE>
class Shader
{
public:
	GLuint id;
	Shader(std::string source);

	void Delete();
	void Compile();
	ShaderNoType Get();
};

class SimpleProgram
{
public:
	GLuint id;

	SimpleProgram();
	void AttachShader(ShaderNoType shdr);
	void Compile();
	
	void Use();
};

template<GLenum TYPE>
inline Shader<TYPE>::Shader(std::string source)
{
	id = glCreateShader(TYPE);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, NULL);
}

template<GLenum TYPE>
void Shader<TYPE>::Delete()
{
	glDeleteShader(id);
}

template<GLenum TYPE>
void Shader<TYPE>::Compile()
{
	glCompileShader(id);
}

template<GLenum TYPE>
ShaderNoType Shader<TYPE>::Get()
{
	return ShaderNoType(id);
}