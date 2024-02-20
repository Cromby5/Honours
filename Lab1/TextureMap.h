#pragma once
#include <string>
#include <GL\glew.h>


class TextureMap
{
public:
	
	TextureMap();
	
    TextureMap(const std::string& fileName);

	//move constructor
	TextureMap(TextureMap&& other) {
		init(other);
	}

	//copy constructor
	TextureMap(const TextureMap& other) {
		init(other);
	}

	TextureMap& operator=(TextureMap&& other)
	{
		if (this == &other)
			return *this;
		return init(other);
	}

	TextureMap& operator=(const TextureMap& other)
	{
		if (this == &other)
			return *this;
		return init(other);
	}

	void LoadTexture(const std::string& fileName, GLuint unit);

	void Bind(unsigned int unit); // bind upto 32 textures

	~TextureMap();

protected:
private:
	enum
	{
		DEFAULT,
		NORMAL,
		SPECULAR,
		NUM_TYPES
	};
	
	GLuint textureID[NUM_TYPES];

	TextureMap& init(const TextureMap& other) {
		textureID[0] = other.textureID[0];
		textureID[1] = other.textureID[1];
		textureID[2] = other.textureID[2];
		return *this;
	}

	TextureMap& init(TextureMap&& other) {
		textureID[0] = other.textureID[0];
		textureID[1] = other.textureID[1];
		textureID[2] = other.textureID[2];
		return *this;
	}
	
};

