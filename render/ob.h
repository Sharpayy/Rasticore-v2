#pragma once
#include <vector>
#include "buffers.h"
#include "locals.h"
#include <unordered_map>
#include <vector>
#include <stack>
#include <list>
#include <unordered_map>

class RenderableObject
{
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	ModelBuffer::Model* source_model;
	uint32_t matrix_id;

	glm::mat4* transform_matrix;
	glm::mat4* transform_matrix_device; // maybe will use

	uint32_t flags;

};

typedef struct _ObjectCreateParameters
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
} ObjectCreateParameters;

typedef struct _RenderObject
{
	uint32_t matrix_index;

} RenderObject;

typedef struct _RenderCommand
{
	GLuint program;
	ModelBuffer* mb;
	uint32_t start;
	uint32_t count;
} RenderCommand;

class LocalObjectStorage
{
public:

	class Container
	{
	public:
		std::unordered_map<ModelBuffer::Model*, uint64_t> models;
		Container() = default;
		Container(ModelBuffer::Model* first);
	};

	class IndirectDrawBuffers
	{
	public:
		Buffer<GL_DRAW_INDIRECT_BUFFER> buffer_indirect;
		Buffer<GL_SHADER_STORAGE_BUFFER> buffer_object;
		RenderCommand* commands;
		uint32_t command_count;

		void Bind();
	};

	class IndirectDrawList
	{
	public:
		std::vector<DrawElementsIndirectCommand> indirect_vector;
		std::vector<RenderObject> object_vector;
		std::vector<RenderCommand> command_vector;

		IndirectDrawBuffers* buffers;
		void Pack(bool invalidate_indirect = false, bool invalidate_object = false);
	};

	std::stack<RenderableObject*> dirty_stack;
	std::list<RenderableObject*> objects;
	std::unordered_map<ModelBuffer*, Container> mb_used;

	LocalObjectStorage() = default;

	void AddModelWithBuffer(ModelBuffer::Model* md, ModelBuffer* mb);
	void RemoveModelWithBuffer(ModelBuffer::Model* md, ModelBuffer* mb);
	void AddNewObject(RenderableObject* ro);
	RenderableObject* RemoveObject(RenderableObject* ob);

	void GenerateIndirectList(ModelBuffer* mb);

	uint32_t indirect_entries;
	IndirectDrawList* draw_list;
};

RenderableObject* ObCreateObject(ModelBuffer* mb, ModelBuffer::Model* md, ObjectCreateParameters* ocp = NULL);
RenderableObject* ObRemoveObject(RenderableObject* ob);

void InitializeObjectStorage();
bool GetLocalObjectStorage(LocalObjectStorage** os);
