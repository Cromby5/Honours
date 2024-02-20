#pragma once
#include <GL/glew.h>
#include <glm\glm.hpp>
#include "WorldCamera.h"
#include "ShaderHandler.h"

using namespace std;

class SkyBox
{
public:
	SkyBox();
	~SkyBox();
	
	void initSkyBox();
	void loadCubeMap();

	void drawSkyBox(const WorldCamera& camera);
	void drawCube(const Transform& transform, const WorldCamera& camera);
	
	unsigned int textureID;
	
private:
	GLuint vbo;
	GLuint vao;
	GLuint ebo;
	unsigned int cubeVAO, cubeVBO;
	ShaderHandler skyShader;
	ShaderHandler reflectShader;
};

