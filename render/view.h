#pragma once
#include "buffers.h"
#include <string>



class IViewCamera
{
public:
	virtual glm::mat4 GetViewMatrix() = 0;
};

class IProjectionProvider
{
public:
	virtual glm::mat4 GetProjMatrix() = 0;
};

class SimplePerspective : public IProjectionProvider
{
public:
	float fov;
	float near_p, far_p;
	float window_x, window_y;

	glm::mat4 pm;

	SimplePerspective(float wx, float wy, float wfov);

	virtual glm::mat4 GetProjMatrix();

	glm::mat4 CalculateProjMatrix();
};

class Camera : public IViewCamera
{
public:
	// inetrface impl
	virtual glm::mat4 GetViewMatrix()
	{
		return getMatrix();
	}

	// old camera
	Camera(glm::vec3 pos)
	{
		yaw = 90.0f; pitch = 0.0f; roll = 0.0f;
		this->pos = pos;

		f = glm::normalize(glm::vec3(
			cos(yaw) * cos(pitch),
			sin(pitch),
			cos(pitch) * sin(yaw)
		));
		r = glm::normalize(glm::cross(f, glm::vec3(0.0f, 1.0f, 0.0f)));
		u = glm::normalize(glm::cross(f, r));
	}
	glm::mat4 getMatrix()
	{
		f = glm::normalize(glm::vec3(
			cos(yaw) * cos(pitch),
			sin(pitch),
			cos(pitch) * sin(yaw)
		));

		glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), roll, f);

		r = glm::normalize(glm::cross(f, glm::vec3(0.0f, 1.0f, 0.0f)));
		u = glm::mat3(roll_mat) * glm::normalize(glm::cross(f, r));
		glm::mat4 M = glm::lookAt(pos, f + pos, u);
		return M;
	}
	glm::vec3 getPos()
	{
		return pos;
	}
	glm::vec3* getPPos()
	{
		return &pos;
	}
	void addYaw(float x)
	{
		yaw += x;
	}
	void addPitch(float x)
	{
		pitch += x;
	}
	void addRoll(float x)
	{
		roll += x;
	}
	void setMatrix(glm::mat4* matrix)
	{
		f = glm::normalize(glm::vec3(
			cos(yaw) * cos(pitch),
			sin(pitch),
			cos(pitch) * sin(yaw)
		));

		glm::mat4 roll_mat = glm::rotate(glm::mat4(1.0f), roll, f);

		r = glm::normalize(glm::cross(f, glm::vec3(0.0f, 1.0f, 0.0f)));
		u = glm::mat3(roll_mat) * glm::normalize(glm::cross(f, r));

		*matrix = glm::lookAt(pos, f + pos, u);
	}
	void setYaw(float yaw)
	{
		this->yaw = yaw;
	}
	void setPitch(float pitch)
	{
		this->pitch = pitch;
	}
	void setRoll(float roll)
	{
		this->roll = roll;
	}
	glm::vec3 getRight()
	{
		return r;
	}
	void setPos(glm::vec3 pos)
	{
		this->pos = pos;
	}
	void addPos(glm::vec3 pos)
	{
		this->pos += pos;
	}
	void posAddFront(float s)
	{
		pos += f * s;
	}
	void posAddRight(float s)
	{
		pos += r * s;
	}

	float yaw, pitch, roll;
	glm::vec3 pos;
	glm::vec3 r, u, f;
};

class MainView
{
private:
	glm::mat4 view_matrix;
	glm::mat4 proj_matrix;
	glm::mat4 PV_matrix;

	IViewCamera* view_camera;
	IProjectionProvider* proj_prov;

public:
	MainView();

	void SetViewMatrix(glm::mat4 m);
	void SetProjMatrix(glm::mat4 m);

	void SetViewCamera(IViewCamera* cam);
	void SetProjProvider(IProjectionProvider* pro);

	void Update();
	void UpdateViewCamera();
	void UpdateProjProvider();
	void WriteData(); // send to gpu side

	void UpdateFull(); // query view and proj then calc and update
	void InitializeBuffer();


	typedef struct _MainView_UniformData
	{
		glm::mat4 view;
		glm::mat4 proj;
		glm::mat4 PV;
	} MainView_UniformData;
	
private:
	UniformBuffer view_uniform;

};

namespace view
{
	MainView* GetMainViewClass();
	void InitializeMainView();
}