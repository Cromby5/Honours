#pragma once
#include <string>
#include <GL\glew.h>
#include "transform.h"
#include "WorldCamera.h"
#include <iostream>
#include <fstream>

class ShaderHandler
{
public:
	ShaderHandler();
	ShaderHandler(const std::string& fileName);

	ShaderHandler(ShaderHandler&& other) {
		init(other);
	}

	ShaderHandler(const ShaderHandler& other) {
		init(other);
	}

	ShaderHandler& operator=(ShaderHandler&& other)
	{
		if (this == &other)
			return *this;
		return init(other);
	}

	ShaderHandler& operator=(const ShaderHandler& other)
	{
		if (this == &other)
			return *this;
		return init(other);
	}


	void Use(); // Set gpu to use our shaders
	//void ParseShader(const std::string& filename); // Parse the shader files to seperate vert and frag shaders
	void Update(const Transform& transform, const WorldCamera& camera, float counter, float newCounter);
	void UpdateSky(const WorldCamera& camera);
	void UpdateLight(const Transform& transform, const WorldCamera& camera);
	
	void init(const std::string& filename);
	void initGeo(); // TEMP for geo shader

	GLuint getID() { return this->program; }

	std::string ShaderHandler::LoadShader(const std::string& fileName);
	void ShaderHandler::CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& errorMessage);
	GLuint ShaderHandler::CompileShader(const std::string& text, unsigned int type);

    ~ShaderHandler();

	// ------------------------------------------------------------------------
	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);

		if ((glGetUniformLocation(program, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(program, name.c_str()), value);

		if ((glGetUniformLocation(program, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(program, name.c_str()), value);

		//if ((glGetUniformLocation(program, name.c_str()) == -1))
		//{
		//std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
		//__debugbreak();
		//}
	}
	// ------------------------------------------------------------------------
	void setVec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);

		if ((glGetUniformLocation(program, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	void setVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(program, name.c_str()), x, y);

		if ((glGetUniformLocation(program, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);

		if ((glGetUniformLocation(program, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z);

		if ((glGetUniformLocation(program, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);

		if ((glGetUniformLocation(program, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	void setVec4(const std::string& name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(program, name.c_str()), x, y, z, w);

		if ((glGetUniformLocation(program, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);

		if ((glGetUniformLocation(program, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);

		if ((glGetUniformLocation(program, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
	// ------------------------------------------------------------------------
	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);

		if ((glGetUniformLocation(program, name.c_str()) == -1))
		{
			std::cerr << "Unable to load shader: " << name.c_str() << std::endl;
			__debugbreak();
		}
	}
protected:
private:
	static const unsigned int NUM_SHADERS = 2; // number of shaders

	enum
	{
		TRANSFORM_U,
		MODEL,
		VIEW,
		PROJECTION,
		CAMERA_POS,
		LIGHT_POS,
		SKYBOX,
		LIGHT_COLOUR,
		DIFFUSE,
		NORMALMAP,
		SPECULAR,
		NUM_UNIFORMS
	};
	
	float counter;

	GLuint program; // Track the shader program
	GLuint shaders[NUM_SHADERS]; //array of shaders
	GLuint shaders1[3]; // Temp for geo shader
	GLuint uniforms[NUM_UNIFORMS]; //no of uniform variables


	ShaderHandler& init(const ShaderHandler& other) {
		program = other.program;
		for (unsigned int i = 0; i < NUM_SHADERS; i++)
			shaders[i] = other.shaders[i];
		for (unsigned int i = 0; i < NUM_UNIFORMS; i++)
			uniforms[i] = other.uniforms[i];
		return *this;
	}

	ShaderHandler& init(ShaderHandler&& other) {
		program = other.program;
		for (unsigned int i = 0; i < NUM_SHADERS; i++)
			shaders[i] = other.shaders[i];
		for (unsigned int i = 0; i < NUM_UNIFORMS; i++)
			uniforms[i] = other.uniforms[i];
		return *this;
	}
};
