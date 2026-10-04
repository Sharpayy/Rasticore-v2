#include "buffers.h"

VertexArray::VertexArray()
{
	id = -1;
}

VertexArray::VertexArray(Buffer<GL_ELEMENT_ARRAY_BUFFER> ebo, Buffer<GL_ARRAY_BUFFER> vbo)
{
	glCreateVertexArrays(1, &id);
	glBindVertexArray(id);

	vbo.Bind();
	ebo.Bind();
}

void VertexArray::Bind()
{
	glBindVertexArray(id);
}

void VertexArray::Enable(uint32_t index)
{
	glEnableVertexArrayAttrib(id, index);
}

void VertexArray::Parameter(VERTEX_ARRAY_PARAMETER_DATA* data)
{
	glVertexAttribPointer(data->index, data->size, data->type, GL_FALSE, data->stride, (const void*)data->offset);
	Enable(data->index);
}

void QueryVertexArrayParameterData(VERTEX_ARRAY_PARAMETER_DATA* data, VertexArrayParameterType type)
{
	switch (type)
	{
		case TypeVertex2Only:
			data[0].index = SHADER_INPUT_LOCATION_VERTEX_POSITION;
			data[0].size = 2;
			data[0].type = GL_FLOAT;
			data[0].stride = 2 * sizeof(float);
			data[0].offset = 0;
			break;
		case TypeVertex3Only:
			data[0].index = SHADER_INPUT_LOCATION_VERTEX_POSITION;
			data[0].size = 3;
			data[0].type = GL_FLOAT;
			data[0].stride = 3 * sizeof(float);
			data[0].offset = 0;
			break;
		case TypeVertex2Texture:
			data[0].index = SHADER_INPUT_LOCATION_VERTEX_POSITION;
			data[0].size = 2;
			data[0].type = GL_FLOAT;
			data[0].stride = 4 * sizeof(float);
			data[0].offset = 0 * sizeof(float);

			data[1].index = SHADER_INPUT_LOCATION_TEXTURE_COORDS;
			data[1].size = 2;
			data[1].type = GL_FLOAT;
			data[1].stride = 4 * sizeof(float);
			data[1].offset = 2 * sizeof(float);
			break;
		case TypeVertex3Texture:
			data[0].index = SHADER_INPUT_LOCATION_VERTEX_POSITION;
			data[0].size = 3;
			data[0].type = GL_FLOAT;
			data[0].stride = 5 * sizeof(float);
			data[0].offset = 0 * sizeof(float);

			data[1].index = SHADER_INPUT_LOCATION_TEXTURE_COORDS;
			data[1].size = 2;
			data[1].type = GL_FLOAT;
			data[1].stride = 5 * sizeof(float);
			data[1].offset = 3 * sizeof(float);
			break;
	}

}

void QueryVertexArrayVertexSize(uint64_t* vertex_size, VertexArrayParameterType type)
{
	switch (type)
	{
	case TypeVertex2Only:
		*vertex_size = sizeof(float) * 2;
		break;
	case TypeVertex3Only:
		*vertex_size = sizeof(float) * 3;
		break;
	case TypeVertex2Texture:
		*vertex_size = sizeof(float) * 4;
		break;
	case TypeVertex3Texture:
		*vertex_size = sizeof(float) * 5;
		break;
	}
}

ModelBuffer::ModelBuffer(uint64_t ebo_size, uint64_t vbo_size, VertexArrayParameterType type)
{
	primitive_render_type = GL_TRIANGLES;
	va_type = type;

	ebo_buffer_max_size = ebo_size;
	vbo_buffer_max_size = vbo_size;

	ebo_buffer_current_size = 0;
	vbo_buffer_current_size = 0;

	//ebo = Buffer<GL_ELEMENT_ARRAY_BUFFER>(ebo_size, NULL, GL_DYNAMIC_STORAGE_BIT);
	//vbo = Buffer<GL_ARRAY_BUFFER>(vbo_size, NULL, GL_DYNAMIC_STORAGE_BIT);

	vertex_data = std::vector<float>();
	indice_data = std::vector<uint32_t>();
	pack_flag = false;

	//va = VertexArray(ebo, vbo);

	//VERTEX_ARRAY_PARAMETER_DATA t[8];
	//QueryVertexArrayParameterData(&t[0], type);

	//va.Parameter(&t[0]);
	//va.Parameter(&t[1]);
}

uint32_t ModelBuffer::AddNewModel(std::vector<float> vertex, std::vector<uint32_t> indice, uint32_t count)
{
	if (vertex.size() * 4 > VertexFreeSpace())
		return -1;

	if (indice.size() * 4 > IndiceFreeSpace())
		return -1;

	uint32_t base_vertex = vbo_buffer_current_size / 4;
	uint32_t base_indice = ebo_buffer_current_size / 4;
	
	vbo_buffer_current_size += vertex.size() * sizeof(float);
	ebo_buffer_current_size += indice.size() * sizeof(uint32_t);

	vertex_data.insert(vertex_data.end(), vertex.begin(), vertex.end());
	indice_data.insert(indice_data.end(), indice.begin(), indice.end());

	Model* model = new Model();
	model->ebo_offset = base_indice;
	model->vbo_offset = base_vertex;
	model->source_model_buffer = this;
	model->vertex_count = count;

	uint32_t id = models.size();
	models.push_back(model);

	return id;
} 

void ModelBuffer::PackBuffers(bool del_cpu_side)
{
	ebo = Buffer<GL_ELEMENT_ARRAY_BUFFER>(ebo_buffer_current_size, (void*)indice_data.data(), GL_DYNAMIC_STORAGE_BIT);
	vbo = Buffer<GL_ARRAY_BUFFER>(vbo_buffer_current_size, (void*)vertex_data.data(), GL_DYNAMIC_STORAGE_BIT);

	va = VertexArray(ebo, vbo);

	VERTEX_ARRAY_PARAMETER_DATA t[8];
	QueryVertexArrayParameterData(&t[0], va_type);
	va.Parameter(&t[0]);

	glBindVertexArray(0);

	pack_flag = true;

	if (del_cpu_side)
	{
		vertex_data.erase(vertex_data.begin(), vertex_data.end());
		indice_data.erase(indice_data.begin(), indice_data.end());
	}
}

void ModelBuffer::BindVa()
{
	va.Bind();
}

uint32_t ModelBuffer::IndiceFreeSpace()
{
	return ebo_buffer_max_size - ebo_buffer_current_size;
}

uint32_t ModelBuffer::VertexFreeSpace()
{
	return ebo_buffer_max_size - vbo_buffer_current_size;
}

GLenum ModelBuffer::GetDrawMode()
{
	return primitive_render_type;
}

ModelBuffer::Model* ModelBuffer::GetModel(uint32_t id)
{
	return models.at(id);
}

bool ModelBuffer::IsPacked()
{
	return pack_flag;
}

VertexArrayParameterType ModelBuffer::GetVaType()
{
	return va_type;
}

size_t ModelBuffer::GetModelCount()
{
	return models.size();
}

void draw::DrawSingleModel(uint32_t model_id, ModelBuffer* mb)
{
	ModelBuffer::Model* md = mb->GetModel(model_id);
	mb->BindVa();

	uint64_t vtx_size = 0;
	QueryVertexArrayVertexSize(&vtx_size, mb->GetVaType());

	glDrawElementsBaseVertex(mb->GetDrawMode(), md->vertex_count, GL_UNSIGNED_INT, (const void*)(md->ebo_offset * sizeof(uint32_t)), md->vbo_offset / (vtx_size / 4));
}

UniformBuffer::UniformBuffer(uint64_t size, void* data, uint32_t binding)
{
	glCreateBuffers(1, &id);
	glNamedBufferData(id, size, data, GL_DYNAMIC_DRAW);

	if (binding != -1)
		BindBase(binding);
}

void UniformBuffer::Bind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, id);
}

void UniformBuffer::BindBase(uint32_t binding)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
}

void UniformBuffer::Write(uint64_t offset, uint64_t size, void* data)
{
	glNamedBufferSubData(id, offset, size, data);
}
