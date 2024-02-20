#include "AudioHandler.h"

AudioHandler::AudioHandler()
{
    int audio_rate = 22050;
    Uint16 audio_format = AUDIO_S16SYS;
    int audio_channels = 2;
    int audio_buffers = 4096;
	backgroundMusic = NULL;

    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't init audio: %s", Mix_GetError());
        exit(-1);
    }
}

AudioHandler::~AudioHandler()
{
    SDL_Quit();
}

void AudioHandler::initAudio()
{
    addSound("..\\res\\Audio\\whistlere.wav");
    addBackMusic("..\\res\\Audio\\background.wav");
}

void AudioHandler::addSound(const char* path)
{
    Mix_Chunk* tmpChunk = Mix_LoadWAV(path);

    if (tmpChunk != nullptr)
    {
        mSoundEffectBank.push_back(tmpChunk);
        std::cout << (mSoundEffectBank.size() - 1) << " Sound is Ready, path: " << path << '\n';
    }
    else
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't init audio: %s", Mix_GetError());
    }
}

void AudioHandler::addBackMusic(const char* path)
{
    backgroundMusic = Mix_LoadMUS(path);

    if (backgroundMusic == NULL)
    {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
    }
}

void AudioHandler::playSound(const int which) const
{
    if (which > mSoundEffectBank.size() - 1)
    {
        std::cout << "Sound out of range.\n";
        return;
    }

    Mix_PlayChannel(-1, mSoundEffectBank[which], 0);

    std::cout << "Played Sound: " << which << '\n';
}

void AudioHandler::playBackMusic()
{
    if (Mix_PlayingMusic() == 0)
    {
        // Play the music 
        Mix_PlayMusic(backgroundMusic, -1);
    }
}
