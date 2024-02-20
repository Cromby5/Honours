#include "GameProcess.h"
#include <iostream>
#include <string>


GameProcess::~GameProcess()
{
}

void GameProcess::run()
{
	initSystems(); 
	gameProcessLoop();
}

void GameProcess::initSystems()
{
	_gameDisplay.initDisplay();
	sky.initSkyBox();
	objectHandler.initObjects();
	gameAudio.initAudio();

	CreateEntitys();

	_gameDisplay.initImgui();

	fbo.initQuad();
	fbo.genFBO(_gameDisplay.getScreenWidth(), _gameDisplay.getScreenHeight());
	fbo2.genFBO(_gameDisplay.getScreenWidth(), _gameDisplay.getScreenHeight());
	fbo.GenGBuffer(_gameDisplay.getScreenWidth(), _gameDisplay.getScreenHeight());
	
	myCamera.initWorldCamera(glm::vec3(0, 0, 5), 70.0f, (float)_gameDisplay.getScreenWidth()/_gameDisplay.getScreenHeight(), 0.1f, 1000.0f);
	myTopDownCamera.initWorldCamera(glm::vec3(0, 20, 5), 70.0f, (float)_gameDisplay.getScreenWidth() / _gameDisplay.getScreenHeight(), 0.01f, 1000.0f);

	counter = 1.0f;
}

void GameProcess::gameProcessLoop()
{
	while (_gameState != GameState::EXIT)
	{
		deltaTime.Update();
		gameAudio.playBackMusic();
		Input();
		_gameDisplay.newFrameImgui();
		drawGame();

		//objectHandler.collision(deltaTime.GetDeltaTime(), gameAudio);
		activeScene.CheckCollisionsEntity(player ,deltaTime.GetDeltaTime());
	}
}

void GameProcess::Input()
{
	SDL_Event event;
	while(SDL_PollEvent(&event)) // Get and process events using the SDL event system 
	{
		switch (event.type)
		{
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				while (deltaTime.GetDeltaTime() > 0) // Key presses will still be registered from the keydown event, however they will only be processed if the deltatime is positive
				{
					case SDLK_SPACE:
						// Shoot Missiles
						InstantiateEntityTest();
						//gameAudio.playSound(0);
						break;
					case SDLK_ESCAPE:
						_gameState = GameState::EXIT;
						break;
					case SDLK_TAB:
						// Open / close imgui window
						_gameDisplay.ToggleImGuiWindow();
						break;
				  /*  case SDLK_p:
						myCamera.SetLook(glm::vec3(0, 0, 0));
						break;*/
				}
			}
			break;
		case SDL_MOUSEMOTION:
			if (_gameDisplay.getImGuiStatus())
				// If the mouse is over an imgui window, do not rotate the camera
				SDL_SetRelativeMouseMode(SDL_FALSE); // Unlock mouse from window and show it on screen
			else
			{
				SDL_SetRelativeMouseMode(SDL_TRUE); // Lock mouse to window and hide it from view 
				myCamera.RotateX((-event.motion.xrel / 1000.0f)); // Rotate camera on X axis (Remove the - on the xrel to invert the rotation)
				myCamera.RotateY((-event.motion.yrel / 1000.0f)); // Rotate camera on Y axis (Remove the - on the yrel to invert the rotation)
			}
		break;
		}
		_gameDisplay.imguiProcessEvent(event); // Start the Dear ImGui frame
	}
	// GET KEYBOARD STATE, for continuous movement as keydown events are only registered once or something that makes it so that the camera only moves once per tick when a key is pressed
	// very smooth, after fixing the deltatime issue
	SDL_PumpEvents();
	keystate = SDL_GetKeyboardState(NULL);
	float mspeed = speed * deltaTime.GetDeltaTime();
	if (keystate[SDL_SCANCODE_A]) {
		myCamera.MoveRight(-mspeed);
	}
	if (keystate[SDL_SCANCODE_D]) {
		myCamera.MoveRight(mspeed);
	}
	if (keystate[SDL_SCANCODE_W]) {
		myCamera.MoveForward(mspeed);
	}
	if (keystate[SDL_SCANCODE_S]) {
		myCamera.MoveForward(-mspeed);
	}
	if (keystate[SDL_SCANCODE_P]) {
		myCamera.SetLook(glm::vec3(0, 0, 0));
	}
}
	

void GameProcess::drawGame()
{
	_gameDisplay.clearDisplayBuffer(0.0f, 0.0f, 0.0f, 1.0f);
	//myCamera.SetLook(glm::vec3(0, 0, 0));
	myTopDownCamera.SetLook(glm::vec3(0, 0, 0));

	fbo.bindFBO(); // Draw to FBO
		sky.drawSkyBox(myTopDownCamera);
		objectHandler.drawObjects(myTopDownCamera,counter,newCount);
		activeScene.Update(deltaTime.GetDeltaTime(), myTopDownCamera, counter, newCount,false);
	fbo.unbindFBO();

	fbo.drawQuad(); // draw fbo to screen
	glEnable(GL_DEPTH_TEST);
	sky.drawSkyBox(myCamera);
	objectHandler.drawObjects(myCamera, counter, newCount);
	activeScene.Update(deltaTime.GetDeltaTime(), myCamera, counter, newCount,true);

	_gameDisplay.renderImgui(); // Render imgui
	counter += deltaTime.GetDeltaTime() * 1.0f;
	
	newCount += 0.1f; // I believe I broke deltatime / or I am blind to a very obvious issu, so this is a really bad temporary fix to display GP CW without taking up more time

	glEnableClientState(GL_COLOR_ARRAY); 
	glEnd();
	_gameDisplay.swapBuffer();
} 


// bloat that can go elsewhere, shows off the ecs though

void GameProcess::CreateEntitys()
{
	// 1
	auto& test = activeScene.CreateEntity("Missile");
	test.AddComponent<Model>("../res/Models/monkey3.obj");
	test.AddComponent<ShaderHandler>("..\\res\\Shaders\\shader");
	test.AddComponent<TextureMap>("..\\res\\Textures\\Oak-Architextures.jpg");
	test.AddComponent<Missile>();
	test.AddComponent<Sphere>();
	test.GetComponent<Transform>().SetPos(glm::vec3(0.0, 0.0, 50.0));


	player = activeScene.CreateEntity("Ship");
	player.AddComponent<Model>("../res/Models/ship.obj");
	player.AddComponent<ShaderHandler>("..\\res\\Shaders\\shader");
	player.AddComponent<TextureMap>("..\\res\\Textures\\Oak-Architextures.jpg");
	player.AddComponent<Sphere>();
	player.AddComponent<Player>();
	player.GetComponent<Transform>().SetPos(glm::vec3(0.0, 0.0, 0.0));
	player.GetComponent<Transform>().SetScale(glm::vec3(0.008, 0.008, 0.008));

	// lets actually have some asteroids in this scene to crash into at least
	for (int i = 0; i < 30; i++)
	{
		auto& test2 = activeScene.CreateEntity("Asteroid");
		test2.AddComponent<Model>("../res/Models/Rock1.obj");
		test2.AddComponent<ShaderHandler>("..\\res\\Shaders\\shader");
		test2.AddComponent<TextureMap>("..\\res\\Textures\\water.jpg");
		test2.AddComponent<Sphere>();
		// random position
		test2.GetComponent<Transform>().SetPos(glm::vec3(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50));
		test2.GetComponent<Transform>().SetScale(glm::vec3(0.1, 0.1, 0.1));
		test2.GetComponent<Sphere>().SetPos(test2.GetComponent<Transform>().GetPos()); // do i really need to do this after?
	}

	// 2
	//auto& test2 = activeScene.CreateEntity("stealing preloads from objhandler goes wrong");
	//test2.AddComponent<Model>(objectHandler.models[0]);
	//test2.AddComponent<ShaderHandler>("..\\res\\Shaders\\shader");
	//test2.AddComponent<TextureMap>(objectHandler.textures[0]);
	//test2.GetComponent<Transform>().SetPos(glm::vec3(0.0, -5.0, 0.0));

}

void GameProcess::InstantiateEntityTest()
{
	auto& missile = activeScene.CreateEntity("Missile");
	missile.AddComponent<Model>("../res/Models/monkey3.obj"); // change this to using preloaded models 
	missile.AddComponent<ShaderHandler>("..\\res\\Shaders\\shader");
	missile.AddComponent<TextureMap>("..\\res\\Textures\\Oak-Architextures.jpg");
	missile.AddComponent<Missile>();
	missile.AddComponent<Sphere>();
	missile.GetComponent<Sphere>().SetPos(missile.GetComponent<Transform>().GetPos());
	missile.GetComponent<Transform>().SetPos(myCamera.getPos() + glm::vec3(0.0f,-2.0f,0.0f)); // going for an underbarrel type thing here
	missile.GetComponent<Transform>().SetForward(myCamera.GetForward());
}