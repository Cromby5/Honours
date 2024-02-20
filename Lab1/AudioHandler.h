#pragma once
#include <SDL/SDL_mixer.h>
#include <SDL/SDL.h>
#include <iostream>
#include <vector>

class AudioHandler
{
	public:
		AudioHandler();
		~AudioHandler();
		
		void initAudio();
		void addSound(const char* path);
		void addBackMusic(const char* path);
		void playSound(const int which) const;
		void playBackMusic();

	private:
		Mix_Music* backgroundMusic;
		std::vector<Mix_Chunk*> mSoundEffectBank;
};

