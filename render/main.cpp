#include <glm/gtc/matrix_transform.hpp>
#include <SDL3/SDL.h>
#include <gl/glew.h>
#include <cstdio>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "buffers.h"
#include "shader.h"
#include "files.h"
#include "images.h"
#include "view.h"
#include "ob.h"
#include "mdi.h"


int main(int argc, char** argv)
{

	SDL_Window* win = SDL_CreateWindow("Test", 500, 500, SDL_WINDOW_OPENGL);
	SDL_Event e;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GLContext ctx = SDL_GL_CreateContext(win);
	SDL_GL_SetSwapInterval(0);
	glewInit();
	view::InitializeMainView();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		"treasure_chest.obj",
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals
	);

	const aiMesh* mesh = scene->mMeshes[0];
	std::vector<float> tc_vtx((float*)mesh->mVertices, ((float*)mesh->mVertices) + mesh->mNumVertices * 3);
	std::vector<uint32_t> tc_ind = std::vector<uint32_t>();
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		auto f = &mesh->mFaces[i];
		tc_ind.push_back(f->mIndices[0]);
		tc_ind.push_back(f->mIndices[1]);
		tc_ind.push_back(f->mIndices[2]);
	}


	std::vector<float> v2 = 
	{
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f

	};

	std::vector<float> v2_2 =
	{
		2.0f, 2.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 2.0f, 0.0f, 0.0f, 1.0f,
		2.0f, 1.0f, 0.0f, 1.0f, 0.0f

	};

	std::vector<uint32_t> i2 =
	{
		0, 1, 2
	};

	Camera cam = Camera(glm::vec3(0.0f, 0.0f, -50.0f));
	SimplePerspective pe = SimplePerspective(500.0f, 500.0f, 90.0f);

	view::GetMainViewClass()->SetViewCamera(&cam);
	view::GetMainViewClass()->SetProjProvider(&pe);

	locals::InitializeThreadStorage();
	InitializeObjectStorage();

	// TODO
	// 
	// 1. Add calculating drawing cpu draw commands +
	// 2. Add textures
	// 3. Create resource manager for shaders and model buffers
	// 4. Sort by shader
	// 5. Do auto drawing from lists +
	// 6. Create persistent matrix buffer
	


	Shader<GL_VERTEX_SHADER> s_v2 = Shader<GL_VERTEX_SHADER>(ReadFile("vtx.glsl"));
	Shader<GL_FRAGMENT_SHADER> s_f2 = Shader<GL_FRAGMENT_SHADER>(ReadFile("frg.glsl"));

	s_v2.Compile();
	s_f2.Compile();

	SimpleProgram p2 = SimpleProgram();
	p2.AttachShader(s_v2.Get());
	p2.AttachShader(s_f2.Get());
	p2.Compile();

	p2.Use();

	ModelBuffer mb3 = ModelBuffer(0x4000, 0x4000, TypeVertex3Only);
	mb3.AddNewModel(tc_vtx, tc_ind, tc_ind.size());
	mb3.PackBuffers(false);

	ModelBuffer mb = ModelBuffer(0x1000, 0x1000, TypeVertex3Texture);
	mb.AddNewModel(v2, i2, 3);
	mb.AddNewModel(v2_2, i2, 3);
	mb.PackBuffers(false);

	auto o1 = ObCreateObject(&mb3, mb3.GetModel(0), nullptr);
	auto o2 = ObCreateObject(&mb, mb.GetModel(0), nullptr);
	auto o3 = ObCreateObject(&mb, mb.GetModel(1), nullptr);

	ObRemoveObject(o2);

	LocalObjectStorage::IndirectDrawBuffers idb;
	idb.buffer_indirect = Buffer<GL_DRAW_INDIRECT_BUFFER>(0, 0, 0);
	idb.buffer_object = Buffer<GL_SHADER_STORAGE_BUFFER>(0, 0, 0);

	LocalObjectStorage* store;
	GetLocalObjectStorage(&store);
	store->GenerateIndirectList(NULL);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	while (true)
	{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		view::GetMainViewClass()->UpdateFull();
		p2.Use();
		//draw::DrawSingleModel(1, &mb);
		//draw::DrawSingleModel(0, &mb);

		store->GenerateIndirectList(NULL);
		store->draw_list->buffers = &idb;
		store->draw_list->Pack();
		mdi::DrawIndirectBuffer(&idb, p2.id);
		//mb3.BindVa();
		//store->draw_list->buffers->Bind();
		//glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, 1, sizeof(DrawElementsIndirectCommand));

		SDL_GL_SwapWindow(win);

		while (SDL_PollEvent(&e));
			

	}

	return 0;
}