#include "Missile.h"

#include <iostream>

Missile::Missile()
{
}

Missile::~Missile()
{
}

void Missile::updateMissile(float deltaTime)
{
	currentLifeTime += 0.5f * deltaTime;
}

bool Missile::CheckLifeTime()
{
	if (currentLifeTime >= lifeTime)
	{
		return true;
	}
	return false;
}
