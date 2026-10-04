#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <SDL3/SDL.h>
#include <gl/glew.h>
#include "enums.h"
#include <vector>
#include <unordered_map>

typedef  struct {
	uint32_t  count;
	uint32_t  instanceCount;
	uint32_t  firstIndex;
	int  baseVertex;
	uint32_t  baseInstance;
} DrawElementsIndirectCommand;

class VERTEX_ARRAY_PARAMETER_DATA
{
public:
	uint32_t index;
	uint32_t size;
	GLenum type;
	uint32_t stride;
	uint32_t offset;
};

void QueryVertexArrayParameterData(VERTEX_ARRAY_PARAMETER_DATA* data, VertexArrayParameterType type);
void QueryVertexArrayVertexSize(uint64_t* vertex_size, VertexArrayParameterType type);

template <GLenum LOCATION>
class Buffer
{
public:
	GLuint id;

	Buffer();
	Buffer(uint64_t size, void* data, GLbitfield access);
	void Bind();
};

class VertexArray
{
public:
	GLuint id;

	VertexArray();
	VertexArray(Buffer<GL_ELEMENT_ARRAY_BUFFER> ebo, Buffer<GL_ARRAY_BUFFER> vbo);
	void Bind();

	void Enable(uint32_t index);
	void Parameter(VERTEX_ARRAY_PARAMETER_DATA* data);
};

class UniformBuffer
{
public:
	GLuint id;

	UniformBuffer() = default;
	UniformBuffer(uint64_t size, void* data, uint32_t binding);
	void Bind();

	void BindBase(uint32_t binding);
	void Write(uint64_t offset, uint64_t size, void* data);
};

class ModelBuffer
{
public:
	class Model
	{
	public:
		uint32_t ebo_offset;
		uint32_t vbo_offset;
		uint32_t vertex_count;
		ModelBuffer* source_model_buffer;

		uint32_t uses;

		Model() = default;

	};

private:
	std::vector<Model*> models;
	
	Buffer<GL_ELEMENT_ARRAY_BUFFER> ebo;
	Buffer<GL_ARRAY_BUFFER> vbo;

	uint64_t ebo_buffer_max_size;
	uint64_t ebo_buffer_current_size;

	uint64_t vbo_buffer_max_size;
	uint64_t vbo_buffer_current_size;

	VertexArray va;
	GLenum primitive_render_type;
	VertexArrayParameterType va_type;

	std::vector<float> vertex_data;
	std::vector<uint32_t> indice_data;

	bool pack_flag;
public:

	ModelBuffer() = default;
	ModelBuffer(uint64_t ebo_size, uint64_t vbo_size, VertexArrayParameterType type);

	uint32_t AddNewModel(std::vector<float> vertex, std::vector<uint32_t> indice, uint32_t count);

	void PackBuffers(bool del_cpu_side);
	void BindVa();

	uint32_t IndiceFreeSpace();
	uint32_t VertexFreeSpace();

	GLenum GetDrawMode();
	Model* GetModel(uint32_t id);
	bool IsPacked();
	VertexArrayParameterType GetVaType();
	size_t GetModelCount();


public:
	
};

namespace draw
{
	void DrawSingleModel(uint32_t model_id, ModelBuffer* mb);
}

template<GLenum LOCATION>
inline Buffer<LOCATION>::Buffer()
{
	id = -1;
}

template<GLenum LOCATION>
inline Buffer<LOCATION>::Buffer(uint64_t size, void* data, GLbitfield access)
{
	glCreateBuffers(1, &id);
	glBindBuffer(LOCATION, id);
	glNamedBufferStorage(id, size, data, access);
}

template<GLenum LOCATION>
void Buffer<LOCATION>::Bind()
{
	glBindBuffer(LOCATION, id);
}