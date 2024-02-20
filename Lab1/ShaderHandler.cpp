#include "ShaderHandler.h"
#include <iostream>
#include <fstream>

ShaderHandler::ShaderHandler()
{
	program = NULL;
	shaders[0] = NULL;
	shaders[1] = NULL;
	
	uniforms[0] = NULL;
	uniforms[1] = NULL;
	uniforms[2] = NULL;
	
}

ShaderHandler::ShaderHandler(const std::string& filename)
{
	init(filename);
}

void ShaderHandler::init(const std::string& filename)
{
	program = glCreateProgram(); // create shader program (openGL saves as ref number)
	shaders[0] = CompileShader(LoadShader(filename + ".vert"), GL_VERTEX_SHADER); // create vertex shader
	shaders[1] = CompileShader(LoadShader(filename + ".frag"), GL_FRAGMENT_SHADER); // create fragment shader

	for (unsigned int i = 0; i < NUM_SHADERS; i++)
	{
		glAttachShader(program, shaders[i]); //add all our shaders to the shader program "shaders"
		glDeleteShader(shaders[i]); // delete the shaders, once attached to the program (they are now saved in the program) 
	}
	// These may be outdated when using version 330 over version 120 due to syntax changes. 
	// e.g. attribute has changed to layout where the location is specified in the shader
	// They are left in for use with version 120 shaders 
	glBindAttribLocation(program, 0, "position"); // associate attribute variable with our shader program attribute (in this case attribute vec3 position;)
	glBindAttribLocation(program, 1, "texCoord");
	glBindAttribLocation(program, 2, "normals");
	

	glLinkProgram(program); //create executables that will run on the GPU shaders
	CheckShaderError(program, GL_LINK_STATUS, true, "Error: Shader program linking failed"); // cheack for error

	glValidateProgram(program); //check the entire program is valid
	CheckShaderError(program, GL_VALIDATE_STATUS, true, "Error: Shader program not valid");

	uniforms[TRANSFORM_U] = glGetUniformLocation(program, "transform"); // associate with the location of uniform variable within a program 
	uniforms[PROJECTION] = glGetUniformLocation(program, "projection");
	uniforms[VIEW] = glGetUniformLocation(program, "view");
	
	uniforms[MODEL] = glGetUniformLocation(program, "model");
	uniforms[CAMERA_POS] = glGetUniformLocation(program, "cameraPos");
	uniforms[NORMALMAP] = glGetUniformLocation(program, "normalMap");

	uniforms[SKYBOX] = glGetUniformLocation(program, "skybox");

	uniforms[LIGHT_COLOUR] = glGetUniformLocation(program, "lightColour");
	uniforms[LIGHT_POS] = glGetUniformLocation(program, "lightPos");

	uniforms[DIFFUSE] = glGetUniformLocation(program, "diffuse");
	uniforms[SPECULAR] = glGetUniformLocation(program, "specular");
	

	glUniform1i(uniforms[SKYBOX], 0);
	glUniform1i(uniforms[DIFFUSE], 0);
	glUniform1i(uniforms[NORMALMAP], 1);
	glUniform1i(uniforms[SPECULAR], 2);
		
}
// Forcing GEO shader to load
void ShaderHandler::initGeo()
{
	program = glCreateProgram(); // create shader program (openGL saves as ref number)
	shaders1[0] = CompileShader(LoadShader("..\\res\\Shaders\\shaderGeoText.vert"), GL_VERTEX_SHADER); // create vertex shader
	shaders1[1] = CompileShader(LoadShader("..\\res\\Shaders\\shaderGeoText.geom"), GL_GEOMETRY_SHADER); // create fragment shader
	shaders1[2] = CompileShader(LoadShader("..\\res\\Shaders\\shaderGeoText.frag"), GL_FRAGMENT_SHADER); // create fragment shader


	for (unsigned int i = 0; i < 3; i++)
	{
		glAttachShader(program, shaders1[i]); //add all our shaders to the shader program "shaders" 
	}

	glBindAttribLocation(program, 0, "VertexPosition"); // associate attribute variable with our shader program attribute (in this case attribute vec3 position;)
	glBindAttribLocation(program, 1, "VertexTexCoord");
	glBindAttribLocation(program, 2, "VertexNormal");

	glLinkProgram(program); //create executables that will run on the GPU shaders
	CheckShaderError(program, GL_LINK_STATUS, true, "Error: Shader program linking failed"); // cheack for error

	glValidateProgram(program); //check the entire program is valid
	CheckShaderError(program, GL_VALIDATE_STATUS, true, "Error: Shader program not valid");

	uniforms[TRANSFORM_U] = glGetUniformLocation(program, "transform"); // associate with the location of uniform variable within a program
}

ShaderHandler::~ShaderHandler()
{
	for (unsigned int i = 0; i < NUM_SHADERS; i++)
	{
		glDetachShader(program, shaders[i]); //detach shader from program
		glDeleteShader(shaders[i]); //delete the sahders
	}
	glDeleteProgram(program); // delete the program
}

void ShaderHandler::Use()
{
	glUseProgram(program); //installs the program object specified by program as part of rendering state
}

void ShaderHandler::Update(const Transform& transform, const WorldCamera& camera,float counter, float newCounter)
{
	glm::mat4 mvp = camera.GetViewProjection() * transform.GetModel();
	glUniformMatrix4fv(uniforms[TRANSFORM_U], 1, GLU_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(uniforms[MODEL], 1, GLU_FALSE, &transform.GetModel()[0][0]);
	glUniformMatrix4fv(uniforms[VIEW], 1, GLU_FALSE, &camera.GetView()[0][0]);
	glUniformMatrix4fv(uniforms[PROJECTION], 1, GLU_FALSE, &camera.GetProjection()[0][0]);
	glUniform3fv(uniforms[CAMERA_POS], 1, &camera.GetPos()[0]);
	
	glm::vec4 lightColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 3.0f, 0.0f);
	glUniform4fv(uniforms[LIGHT_COLOUR], 1, &lightColour[0]);
	glUniform3fv(uniforms[LIGHT_POS], 1, &lightPos[0]);

	// Geo Shader, ideally we move all this stuff somewhere else to seperate these
	float randX = ((float)rand() / (RAND_MAX));
	float randY = ((float)rand() / (RAND_MAX));
	float randZ = ((float)rand() / (RAND_MAX));
	// Frag: uniform float randColourX; uniform float randColourY; uniform float randColourZ;
	setFloat("randColourX", randX);
	setFloat("randColourY", randY);
	setFloat("randColourZ", randZ);
	// Geom: uniform float time, using deltatime;
	setFloat("time", counter);
	setFloat("mandelTime", newCounter);
	//ShaderHandler::UpdateSky(camera);
	// New emapping
}

void ShaderHandler::UpdateSky(const WorldCamera& camera)
{
	// vp remove translation
	glm::mat4 view = glm::mat4(glm::mat3(camera.GetView()));
	glm::mat4 projection = camera.GetProjection();
	glUniformMatrix4fv(uniforms[VIEW], 1, GLU_FALSE, &view[0][0]);
	glUniformMatrix4fv(uniforms[PROJECTION], 1, GLU_FALSE, &projection[0][0]);
	
	glUniform1i(uniforms[SKYBOX], 0);
}

void ShaderHandler::UpdateLight(const Transform& transform, const WorldCamera& camera)
{
	glUniform3fv(uniforms[CAMERA_POS], 1, &camera.GetPos()[0]);
	glm::vec4 lightColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4fv(uniforms[LIGHT_COLOUR], 1, &lightColour[0]);
	glUniform3fv(uniforms[LIGHT_POS], 1, &transform.GetPos()[0]);

}

GLuint ShaderHandler::CompileShader(const std::string& text, unsigned int type)
{
	GLuint shader = glCreateShader(type); //create shader based on specified type

	if (shader == 0) //if == 0 shader no created
		std::cerr << "Error type creation failed " << type << std::endl;

	const GLchar* stringSource[1]; //convert strings into list of c-strings
	stringSource[0] = text.c_str();
	GLint lengths[1];
	lengths[0] = text.length();

	glShaderSource(shader, 1, stringSource, lengths); //send source code to opengl
	glCompileShader(shader); //get open gl to compile shader code

	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!"); //check for compile error

	return shader;
}

std::string ShaderHandler::LoadShader(const std::string& fileName)
{
	std::ifstream file;
	file.open((fileName).c_str());

	std::string output;
	std::string line;

	if (file.is_open())
	{
		while (file.good())
		{
			getline(file, line);
			output.append(line + "\n");
		}
	}
	else
	{
		std::cerr << "Unable to load shader: " << fileName << std::endl;
	}

	return output;
}

void ShaderHandler::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (isProgram)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (success == GL_FALSE)
	{
		if (isProgram)
			glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), NULL, error);

		std::cerr << errorMessage << ": '" << error << "'" << std::endl;
	}
}

