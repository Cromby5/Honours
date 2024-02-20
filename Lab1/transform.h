#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "WorldCamera.h"

struct Transform
{
public:
	Transform(const glm::vec3& pos = glm::vec3(), const glm::vec3& rot = glm::vec3(), const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		this->pos = pos;
		this->rot = rot;
		this->scale = scale;

		this->forward = glm::vec3(0.0f, 0.0f, -1.0f);
	}

	inline glm::mat4 GetModel() const
	{
		glm::mat4 posMat = glm::translate(pos);
		glm::mat4 scaleMat = glm::scale(scale);
		glm::mat4 rotX = glm::rotate(rot.x, glm::vec3(1.0, 0.0, 0.0));
		glm::mat4 rotY = glm::rotate(rot.y, glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 rotZ = glm::rotate(rot.z, glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 rotMat = rotX * rotY * rotZ;

		return posMat * rotMat * scaleMat;
	}

	/*inline glm::mat4 GetMVP(const Camera& camera) const
	{
		glm::mat4 VP = camera.GetViewProjection();
		glm::mat4 M = GetModel();

		return VP * M;//camera.GetViewProjection() * GetModel();
	}*/
	inline glm::vec3 GetPos() const { return pos; } //getters
	inline glm::vec3 GetRot() const { return rot; }
	inline glm::vec3 GetScale() const { return scale; }

	inline void SetPos(glm::vec3& pos) { this->pos = pos; } // setters
	inline void SetRot(glm::vec3& rot) { this->rot = rot; }
	inline void SetScale(glm::vec3& scale) { this->scale = scale; }
	inline void SetForward(glm::vec3& forward) { this->forward = forward; }

	void MoveForward(float speed)
	{
		pos += forward * speed;
	}

	void MoveRight(float speed)
	{
		pos += glm::normalize(glm::cross(forward, up)) * speed;
	}


protected:
private:
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;


	glm::vec3 forward;
	glm::vec3 up;
};


