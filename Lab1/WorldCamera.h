#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "DeltaTime.h"

struct WorldCamera
{
public:
	WorldCamera()
	{
	}

	void initWorldCamera(const glm::vec3& pos, float fov, float aspect, float nearClip, float farClip)
	{
		this->pos = pos;
		this->forward = glm::vec3(0.0f, 0.0f, -1.0f);
		this->up = glm::vec3(0.0f, 1.0f, 0.0f);
		this->projection = glm::perspective(fov, aspect, nearClip, farClip);
	}

	glm::vec3 getPos()
	{
		return this->pos;
	}

	inline glm::vec3 GetPos() const
	{
		return pos;
	}


	inline glm::vec3 GetForward() const
	{
		return forward;
	}

	inline glm::mat4 GetViewProjection() const
	{
		return projection * glm::lookAt(pos, pos + forward, up);
	}
	
	inline glm::mat4 GetView() const
	{
		return glm::lookAt(pos, pos + forward, up);
	}
	
	inline glm::mat4 GetProjection() const
	{
		return projection;
	}
	
	void MoveForward(float speed)
	{
		pos += forward * speed;
	}

	void MoveRight(float speed)
	{
		pos += glm::normalize(glm::cross(forward, up)) * speed;
	}

	void SetLook(glm::vec3 modelPos)
	{
		forward = glm::vec3(glm::normalize(modelPos - pos));
	}

	void RotateY(float angle)
	{
		glm::vec3 right = glm::normalize(glm::cross(forward, up));

		forward = glm::vec3(glm::normalize(glm::rotate(angle, right) * glm::vec4(forward, 0.0)));
		up = glm::normalize(glm::cross(right, forward));
	}

	void RotateX(float angle)
	{
		static const glm::vec3 UP(0.0f, 1.0f, 0.0f);

		glm::mat4 rotation = glm::rotate(angle, UP);

		forward = glm::vec3(glm::normalize(rotation * glm::vec4(forward, 0.0)));
		up = glm::vec3(glm::normalize(rotation * glm::vec4(up, 0.0)));
	}

	// Get pitch / yaw
	inline float GetPitch() const
	{
		return glm::degrees(glm::asin(forward.y));
	}

	inline float GetYaw() const
	{
		return glm::degrees(glm::atan(forward.x, forward.z));
	}

protected:
private:
	glm::mat4 projection;
	glm::vec3 pos;
	glm::vec3 forward;
	glm::vec3 up;
};


