#pragma once
#include <SDL.h>
#include <GL/glew.h>
#include <vector>
#include "DisplayWindow.h" 
#include "transform.h"
#include "AudioHandler.h"
#include "WorldCamera.h"
#include "SkyBox.h"
#include "ObjectHandler.h"
#include "Deltatime.h"
#include "FBO.h"

#include "Missile.h"
#include "Player.h"

#include "scene.h"
#include "Entity.h"

enum class GameState{PLAY, EXIT};

class GameProcess
{
public:
	GameProcess() : _gameState(GameState::PLAY) , keystate(SDL_GetKeyboardState(NULL)) {};
	~GameProcess();

	void run();

private:

	void initSystems();
	void Input();
	void gameProcessLoop();
	void drawGame();


	void CreateEntitys();
	void InstantiateEntityTest();

	DisplayWindow _gameDisplay;
	GameState _gameState;
	WorldCamera myCamera;
	WorldCamera myTopDownCamera;
	
	AudioHandler gameAudio;

	Scene activeScene;
	
	SkyBox sky;
	ObjectHandler objectHandler;
	DeltaTime deltaTime;
	FBO fbo;
	FBO fbo2;

	Entity player;

	float counter = 0.0f;
	float newCount = 0.0f;
	float speed = 4.0f;

	const Uint8* keystate;
};

