#include "DeltaTime.h"


DeltaTime::DeltaTime()
{
}

DeltaTime::~DeltaTime()
{
}


void DeltaTime::Update()
{
	// deltatime
	lastTicks = currentTicks;
	currentTicks = SDL_GetPerformanceCounter();
	m_deltaTime = ((float)(currentTicks - lastTicks) / (double)SDL_GetPerformanceFrequency());; // deltatime values are in ms
}

float DeltaTime::GetDeltaTime() const
{
	return m_deltaTime;
}
