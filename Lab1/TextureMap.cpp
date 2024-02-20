#include "TextureMap.h"
#include "stb_image.h"
#include <cassert>
#include <iostream>

TextureMap::TextureMap()
{
	textureID[0] = 0;
	textureID[1] = 0;
	textureID[2] = 0;
}

 TextureMap::TextureMap(const std::string& fileName)
{
	 LoadTexture(fileName, 0);
}

TextureMap::~TextureMap()
{
	for (int i = 0; i < NUM_TYPES; i++)
	{
		glDeleteTextures(1, &textureID[i]);
	}
}

void TextureMap::LoadTexture(const std::string& fileName, GLuint unit)
{
	int width, height, numComponents; //width, height, and no of components of image
	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* imageData = stbi_load((fileName).c_str(), &width, &height, &numComponents, 4); //load the image and store the data

	if (imageData == NULL)
	{
		std::cerr << "texture load failed" << fileName << std::endl;
	}

	glGenTextures(1, &textureID[unit]); // number of and address of textures
	glActiveTexture(GL_TEXTURE0 + unit); //set acitve texture unit 
	
	glBindTexture(GL_TEXTURE_2D, textureID[unit]); //bind texture - define type 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // wrap texture outside width
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // wrap texture outside height

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // linear filtering for minification (texture is smaller than area)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // linear filtering for magnifcation (texture is larger)

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f); //anisotropic filtering

	glGenerateMipmap(GL_TEXTURE_2D);
	
	// Normal map
	switch (unit)
	{
	case 0:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData); // load image data into texture
		break;
	case 1:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData); // load image data into texture (normal map)
		break;
	case 2:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, imageData); // load image data into texture (specular map)
		break;
	}
		
	stbi_image_free(imageData); // Free up the image memory
	
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
}

void TextureMap::Bind(unsigned int unit)
{
	assert(unit >= 0 && unit <= 31); /// check we are working with one of the 32 textures
	assert(unit >= 0 && unit <= NUM_TYPES); 
	glActiveTexture(GL_TEXTURE0 + unit); // set acitve texture unit
	glBindTexture(GL_TEXTURE_2D, textureID[unit]); // type of and texture to bind to unit
	glActiveTexture(GL_TEXTURE0); // set active texture unit back to 0 once bound to free up memory
}


