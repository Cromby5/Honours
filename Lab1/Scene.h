#pragma once
#include <EnTT/entt.hpp>
#include "transform.h"
#include "MeshHandler.h"
#include "TextureMap.h"
#include "ShaderHandler.h"

#include "ObjectHandler.h" // really forced this in here

#include "Missile.h" // This too, would do a component header file later, have scriptable things like this inhert from a base. like unity with monobehaviour
#include "Player.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

// using The Cherno's Game Engine series, ECS portion https://www.youtube.com/watch?v=D4hz0wEB978
class Entity; // forward declaration of Entity class 

class Scene
{
public:
	Scene();
	~Scene();

	void Update(float deltaTime, const WorldCamera& cam, float counter, float newCounter, bool imgui);

	void CheckCollisionsEntity(Entity entity, float deltaTime); // entity here represents our player for now, doing this to force everything else to get destroyed when hit by them, ignore everything else

	void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f); // for imgui, ideally make a imgui class to handle all of this

	Entity CreateEntity(const std::string& name = std::string()); // the = std::string() is a default parameter, so if no name is passed in, it will be an empty string

private:
	entt::registry m_registry;

	friend class Entity;

	////how to ruin the start of a good system
	//TextureMap textureTemp;
	//ShaderHandler shaderTemp;
	//Model modelTemp;
 //
	//// Arrays of all possible tex/shaders/meshes an Entity can have,  
	//std::vector<TextureMap> textures;
	//std::vector<ShaderHandler> shaders;
	//std::vector<Model> models;
};