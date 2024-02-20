#pragma once

#include <SDL/SDL_timer.h>

class DeltaTime
{

	public:

		DeltaTime();
		~DeltaTime();

		void Update();

		float GetDeltaTime() const;

	private:

		float m_deltaTime = 0.0f;
		Uint64 lastTicks = 0.0f;
		Uint64 currentTicks = SDL_GetPerformanceCounter(); // Cannot believe I left this broken for like a year at this points
};