 #include "DisplayWindow.h"

DisplayWindow::DisplayWindow()
{
	sdlWindow = nullptr; // initialise to generate null access violation for debugging. 
	screenWidth = 1920.0f;
	screenHeight = 1080.0f; 
	glContext = nullptr;
}

DisplayWindow::~DisplayWindow()
{
	SDL_GL_DeleteContext(glContext); // delete context
	SDL_DestroyWindow(sdlWindow); // detete window (make sure to delete the context and the window in the opposite order of creation in initDisplay())
	SDL_Quit();
}

float DisplayWindow::getScreenWidth() { return screenWidth; } // getters
float DisplayWindow::getScreenHeight() { return screenHeight; }
SDL_Window* DisplayWindow::getWindow() { return sdlWindow; }
bool DisplayWindow::getImGuiStatus() { return show_imgui; }

void DisplayWindow::returnError(std::string errorString)
{
	// Show the error message and quit the program
	std::cout << errorString << std::endl;
	std::cout << "press any  key to quit...";
	int in;
	std::cin >> in;
	SDL_Quit();
}

void DisplayWindow::swapBuffer()
{
	SDL_GL_SwapWindow(sdlWindow); // swap buffers 
}

void DisplayWindow::clearDisplayBuffer(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear colour and depth buffer - set colour to colour defined in glClearColor
}

void DisplayWindow::initDisplay()
{
	SDL_Init(SDL_INIT_EVERYTHING); // Initalise everything, not needed???

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); // Min no of bits used to diplay colour
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);// Set up z-buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Set up double buffer   

	sdlWindow = SDL_CreateWindow("Game Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (int)screenWidth, (int)screenHeight, SDL_WINDOW_OPENGL); // create window

	if (sdlWindow == nullptr)
	{
		returnError("window failed to create");
	}

	glContext = SDL_GL_CreateContext(sdlWindow);

	if (glContext == nullptr)
	{
		returnError("SDL_GL context failed to create");
	}

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		returnError("GLEW failed to initialise");
	}

	glEnable(GL_DEPTH_TEST); // enable z-buffering 
	glEnable(GL_CULL_FACE); // dont draw faces that are not pointing at the camera
	glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);

	glEnable(GL_STENCIL_TEST); // enable stencil buffer
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // set stencil buffer to replace values with 1 when the stencil test passes
	
	glEnable(GL_FOG); // Enable fog
	
	glEnable(GL_LIGHTING); // enable lighting
	glEnable(GL_LIGHT0); // enable light 0
	glEnable(GL_COLOR_MATERIAL); // enable colour material

	SDL_GL_SetSwapInterval(Vsync); // set to 0 to disable vsync
	
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
}

#pragma region imgui
void DisplayWindow::initImgui()
{
	//// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(sdlWindow, glContext);
	ImGui_ImplOpenGL3_Init();
	cout << "IMGUI:: Init is complete!" << endl;
}

void DisplayWindow::imguiProcessEvent(SDL_Event& event)
{
	//// (Where your code calls SDL_PollEvent())
	ImGui_ImplSDL2_ProcessEvent(&event); // Forward your event to backend
}

void DisplayWindow::newFrameImgui()
{
	// (After event loop)
	// Start the Dear ImGui frame
	//if (show_imgui)
	//{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	if (show_imgui)
	{
		if (show_demo_window)
		{
			ImGui::ShowDemoWindow(); // Show demo window! :)
		}
		if (show_custom_window)
		{
			myCustomImguiWindow();
		}
	}
		//cout << "IMGUI:: NEWFRAME is complete!" << endl;
	//}
}


void DisplayWindow::renderImgui()
{
	//if (show_imgui)
		//// Rendering
		//// (Your code clears your framebuffer, renders your other stuff etc.)
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//// (Your code calls SDL_GL_SwapWindow() etc.)
		//cout << "IMGUI:: RENDER is complete!" << endl;
}

void DisplayWindow::clearImgui()
{
	//// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void DisplayWindow::ToggleImGuiWindow()
{
	show_imgui = !show_imgui;
}

void DisplayWindow::myCustomImguiWindow()
{
	if (ImGui::CollapsingHeader("Help"))
	{
		ImGui::SeparatorText("ABOUT THIS DEMO:");
		ImGui::BulletText("This is coursework for the Games Programming 3 module, a basic asteroids game made using c++, OpenGL and SDL");
		ImGui::BulletText("This Coursework contains the following extension material");

		ImGui::SeparatorText("IMGUI");
		ImGui::BulletText("A custom imgui window, as seen here allowing some values to be altered");
		ImGui::BulletText("I wanted this for some basic scene editing stuff, very experimental");
		ImGui::BulletText("Press TAB to open / close this window");

		ImGui::SeparatorText("ASSIMP");
		ImGui::BulletText("Assimp Model Loading Integration");
		ImGui::BulletText("Updated SDL + GLEW versions to support x64, allowing assimp to function");
		ImGui::BulletText("stb_image has also been updated to only require its header file");
		ImGui::BulletText("Mesh, Texture class had to be updated to remove any relation to the old obj loader");
		ImGui::BulletText("Essentially a rewrite of those classes to reimplement prior functions");

		ImGui::SeparatorText("EnTT ECS");
		ImGui::BulletText("Entity Component System using the EnTT library");
		ImGui::BulletText("A basic implementation of an ECS, with a scene class and entity class");
		ImGui::BulletText("It currently experiencing bugs so this may be mixed with the old model loading methods to present the work");
		ImGui::BulletText("Textures are broken with the scene rendering method, a bug that I could not fix in time, we can see textures working as normal elsewhere");


		ImGui::SeparatorText("GENERAL");
		ImGui::BulletText("Better responsiveness on movement keys while held down,using SDL_GetKeyboardState");
		ImGui::BulletText("Attempts to consider memory management, smart pointers etc used when possible / when rewriting areas");
		ImGui::BulletText("Move / copy constructors, initilazation lists, operators");
	}
	ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our windows open/close state
	ImGui::Checkbox("Vsync", &Vsync);      
	if (Vsync)
	{
		SDL_GL_SetSwapInterval(1);
	}
	else
	{
		SDL_GL_SetSwapInterval(0);
	}

	//// 1. Show a simple window
	//// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug"
	//{
	//	static float f = 0.0f;
	//	static int counter = 0;

	//	ImGui::Text("Hello, world!");                           // Display some text (you can use a format strings too)
	//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
	//	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	//	if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
	//		counter++;
	//	ImGui::SameLine();
	//	ImGui::Text("counter = %d", counter);

	//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	//	// 2. Show another simple window, this time using an explicit Begin/End pair
	//	if (show_another_window)
	//	{
	//		ImGui::Begin("Another Window", &show_another_window);
	//		ImGui::Text("Hello from another window!");
	//		ImGui::End();
	//	}
	//}
}

#pragma endregion
