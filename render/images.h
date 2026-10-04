#pragma once
#include <GL/glew.h>
#include <string>
#include <windows.h>
#include "shader.h"

#include <stb-master/stb_image.h>

class MappedFile
{
public:
	HANDLE file;
	HANDLE map;
	LPVOID data;

	MappedFile(std::string name);
};

class Image
{
private:
	char* data;
	
public:
	int x, y, c;
	Image(std::string name);

	char* GetData();
};

class Texture2d
{
public:
	GLuint id;
};



class Texture2dArray
{
public:
	GLuint id;
};

void BindProgramSampler2d(GLuint program, std::string sampler_name, GLuint tex_unit);