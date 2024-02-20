#pragma once
#include <SDL/SDL.h>
#include <GL\glew.h>

#include <iostream>
#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

using namespace std;

class DisplayWindow
{
public:
	DisplayWindow();
	~DisplayWindow();
	void initDisplay();
	void swapBuffer();
	void clearDisplayBuffer(float r, float g, float b, float a);

	float getScreenWidth();
	float getScreenHeight();
	bool  getImGuiStatus();
	SDL_Window* getWindow();

	//imgui
	void initImgui();
	void renderImgui();
	void clearImgui();
	void imguiProcessEvent(SDL_Event& event);
	void newFrameImgui(); // start of each loop
	void myCustomImguiWindow();

	void ToggleImGuiWindow();

private:

	void returnError(std::string errorString);
	
	SDL_GLContext glContext; // global variable to hold the context
	SDL_Window* sdlWindow; // holds pointer to out window
	float screenWidth;
	float screenHeight;
	bool show_imgui = true;
	bool show_demo_window = false;
	bool show_custom_window = true;
	bool Vsync = true;
};

