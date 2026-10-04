#include "view.h"

MainView g_MainView;


MainView::MainView()
{
	view_matrix = glm::mat4(1.0f);
	proj_matrix = glm::mat4(1.0f);
	PV_matrix = glm::mat4(1.0f);

	view_camera = nullptr;
	proj_prov = nullptr;

	//view_uniform = UniformBuffer(sizeof(MainView_UniformData), NULL, UNIFORM_BUFFER_BINDING_MAIN_VIEW);
}

void MainView::SetViewMatrix(glm::mat4 m)
{
	view_matrix = m;
}

void MainView::SetProjMatrix(glm::mat4 m)
{
	proj_matrix = m;
}

void MainView::SetViewCamera(IViewCamera* cam)
{
	view_camera = cam;
}

void MainView::SetProjProvider(IProjectionProvider* pro)
{
	proj_prov = pro;
}

void MainView::Update()
{
	PV_matrix = proj_matrix * view_matrix;
}

void MainView::UpdateViewCamera()
{
	if (view_camera != nullptr)
		SetViewMatrix(view_camera->GetViewMatrix());
}

void MainView::UpdateProjProvider()
{
	if (proj_prov != nullptr)
		SetProjMatrix(proj_prov->GetProjMatrix());
}

void MainView::WriteData()
{
	MainView_UniformData data;
	data.proj = proj_matrix;
	data.view = view_matrix;
	data.PV = PV_matrix;

	view_uniform.Write(0, sizeof(MainView_UniformData), &data);
}

void MainView::UpdateFull()
{
	UpdateViewCamera();
	UpdateProjProvider();
	Update();
	WriteData();
}

void MainView::InitializeBuffer()
{
	view_uniform = UniformBuffer(sizeof(MainView_UniformData), NULL, UNIFORM_BUFFER_BINDING_MAIN_VIEW);
}

MainView* view::GetMainViewClass()
{
	return &g_MainView;
}

void view::InitializeMainView()
{
	g_MainView = MainView();
	g_MainView.InitializeBuffer();
}

SimplePerspective::SimplePerspective(float wx, float wy, float wfov)
{
	fov = wfov;
	window_x = wx;
	window_y = wy;

	near_p = 0.1f;
	far_p = 1000.0f;

	pm = CalculateProjMatrix();
}

glm::mat4 SimplePerspective::GetProjMatrix()
{
	return pm;
}

glm::mat4 SimplePerspective::CalculateProjMatrix()
{
	pm = glm::perspective(fov, window_x / window_y, near_p, far_p);
	return pm;
}
