#include "files.h"

std::string ReadFile(const std::string& filename)
{
	std::ifstream file(filename);

	if (!file)
		throw std::runtime_error("Failed to open file: " + filename);

	std::stringstream buffer;
	buffer << file.rdbuf();

	return buffer.str();
}