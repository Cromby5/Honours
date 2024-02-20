#pragma once

#include <glm/glm.hpp>

class Player
{
public:

	Player();
	~Player();

	void MoveForward(float speed, float deltatime);
	void MoveRight(float speed, float deltatime);

private:
	glm::vec3 pos;
	glm::vec3 forward;
	glm::vec3 up;
};
