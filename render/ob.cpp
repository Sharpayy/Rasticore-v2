#include "ob.h"

RenderableObject* ObCreateObject(ModelBuffer* mb, ModelBuffer::Model* md, ObjectCreateParameters* ocp)
{
    RenderableObject* ob = new RenderableObject();

    ob->flags = OBJECT_FLAG_CREATION_STD;
    ob->transform_matrix = new glm::mat4(1.0f);
    ob->transform_matrix_device = nullptr;
    
    if (ocp != NULL)
    {
        ob->position = ocp->position;
        ob->scale = ocp->scale;
        ob->rotation = ocp->rotation;
    }

    ob->source_model = md;
    md->uses++;

    LocalObjectStorage* os;
    GetLocalObjectStorage(&os);

    os->AddNewObject(ob);

    return ob;
}

RenderableObject* ObRemoveObject(RenderableObject* ob)
{
    LocalObjectStorage* os;
    GetLocalObjectStorage(&os);
    os->RemoveObject(ob);

    return ob;
}

void InitializeObjectStorage()
{
    if (locals::g_LocalObjectStorageIndex == -1)
        throw 100; // Initialize tls

    LPVOID val = TlsGetValue(locals::g_LocalObjectStorageIndex);
    if (val != NULL)
        throw 101; // initialized already

    LocalObjectStorage* os = new LocalObjectStorage();
    TlsSetValue(locals::g_LocalObjectStorageIndex, os);
}

bool GetLocalObjectStorage(LocalObjectStorage** os)
{
    *os = (LocalObjectStorage*)TlsGetValue(locals::g_LocalObjectStorageIndex);
    return true;
}



void LocalObjectStorage::AddModelWithBuffer(ModelBuffer::Model* md, ModelBuffer* mb)
{
    // check
    if (md->source_model_buffer != mb)
        throw 5;

    auto mb_f = mb_used.find(mb);

    if (mb_f == mb_used.end())
    {
        mb_used.insert({ mb, Container(md) });
        indirect_entries++;
    }
    else
    {
        auto md_f = mb_f->second.models.find(md);
        if (md_f == mb_f->second.models.end())
        {
            mb_f->second.models.insert({ md, 1 });
            indirect_entries++;
        }
        else
        {
            md_f->second++;
        }
    }
}

void LocalObjectStorage::RemoveModelWithBuffer(ModelBuffer::Model* md, ModelBuffer* mb)
{
    // check
    if (md->source_model_buffer != mb)
        throw 5;

    auto mb_f = mb_used.find(mb);

    if (mb_f == mb_used.end())
        throw "Model buffer missing";

    auto md_f = mb_f->second.models.find(md);

    if (md_f == mb_f->second.models.end())
        throw "Model missing";

    md_f->second--;

    if (md_f->second == 0)
        mb_f->second.models.erase(md_f->first);

    if (mb_f->second.models.size() == 0)
        mb_used.erase(mb);

}

void LocalObjectStorage::AddNewObject(RenderableObject* ro)
{
    AddModelWithBuffer(ro->source_model, ro->source_model->source_model_buffer);

    objects.push_back(ro);
}

// remove objects without check
RenderableObject* LocalObjectStorage::RemoveObject(RenderableObject* ob)
{
    objects.remove(ob);
    RemoveModelWithBuffer(ob->source_model, ob->source_model->source_model_buffer);

    return ob;
}

void LocalObjectStorage::GenerateIndirectList(ModelBuffer* mb)
{
    if (draw_list == NULL)
        draw_list = new IndirectDrawList();

    draw_list->indirect_vector.clear();
    draw_list->indirect_vector.reserve(indirect_entries);

    draw_list->command_vector.clear();


    std::unordered_map<ModelBuffer::Model*, uint64_t> base_instance;
    uint64_t base_instance_counter = 0;

    // create indirect command list
    uint32_t count = 0;
    uint32_t offset = 0;
    for (auto i = mb_used.begin(); i != mb_used.end(); i++)
    {
        uint64_t vtx_size = 0;
        QueryVertexArrayVertexSize(&vtx_size, i->first->GetVaType());
        auto cont = &i->second;
        for (auto mdl = cont->models.begin(); mdl != cont->models.end(); mdl++)
        {
            DrawElementsIndirectCommand cmd;
            cmd.baseInstance = base_instance_counter;
            cmd.baseVertex = mdl->first->vbo_offset / (vtx_size / 4);
            cmd.count = mdl->first->vertex_count;
            cmd.instanceCount = mdl->second;
            cmd.firstIndex = mdl->first->ebo_offset;

            base_instance.insert({ mdl->first, base_instance_counter });
            base_instance_counter += mdl->second;

            draw_list->indirect_vector.push_back(cmd);
            count++;
        }
        
        draw_list->command_vector.push_back(RenderCommand{ 0, i->first, offset, count });
        offset += count;
        count = 0;
    }

    draw_list->object_vector.clear();
    draw_list->object_vector.resize(objects.size());

    // create matrix id list 
    for (auto obj = objects.begin(); obj != objects.end(); obj++)
    {
        draw_list->object_vector.data()[++base_instance.find((*obj)->source_model)->second] = RenderObject{ (*obj)->matrix_id };
    }
    
}

LocalObjectStorage::Container::Container(ModelBuffer::Model* first)
{
    models = std::unordered_map<ModelBuffer::Model*, uint64_t>();
    models.insert({ first, 1 });
}

void LocalObjectStorage::IndirectDrawList::Pack(bool invalidate_indirect, bool invalidate_object)
{
    if (buffers == NULL)
        return;

    if (invalidate_indirect == true)
        glInvalidateBufferData(buffers->buffer_indirect.id);

    if (invalidate_object == true)
        glInvalidateBufferData(buffers->buffer_object.id);

    glNamedBufferData(buffers->buffer_indirect.id, indirect_vector.size() * sizeof(DrawElementsIndirectCommand), indirect_vector.data(), GL_DYNAMIC_DRAW);
    glNamedBufferData(buffers->buffer_object.id, object_vector.size() * sizeof(RenderObject), object_vector.data(), GL_DYNAMIC_DRAW);

    buffers->command_count = command_vector.size();
    buffers->commands = command_vector.data();
}

void LocalObjectStorage::IndirectDrawBuffers::Bind()
{
    buffer_indirect.Bind();
    buffer_object.Bind();
}
