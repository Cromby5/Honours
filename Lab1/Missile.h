#pragma once

class Missile
{
public:

	Missile();
	~Missile();

	void updateMissile(float deltaTime);

	bool CheckLifeTime();

	float lifeTime = 7.0f;
private:
	float speed = 0.0f;
	float currentLifeTime = 0.0f;


};