#include "images.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>

MappedFile::MappedFile(std::string name)
{
	file = CreateFileA(name.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return;

	map = CreateFileMappingA(file, NULL, PAGE_WRITECOPY, 0, 0, ("Local\\mfres\\" + name).c_str());
	auto f = GetLastError();
	if (map == NULL)
		return;

	data = MapViewOfFile(map, FILE_MAP_ALL_ACCESS, 0, 0, 0);
}

Image::Image(std::string name)
{
	data = (char*)stbi_load(name.c_str(), &x, &y, &c, 4);
}

char* Image::GetData()
{
	return data;
}

void BindProgramSampler2d(GLuint program, std::string sampler_name, GLuint tex_unit)
{
	glUseProgram(program);
	uint32_t loc = glGetUniformLocation(program, sampler_name.c_str());

	glUniform1i(loc, tex_unit);
}
