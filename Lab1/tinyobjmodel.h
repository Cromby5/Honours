#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
//#include <tinyObj/tiny_obj_loader2.h>
#include "MeshHandler.h"
//#include "Clock.h"

class tinyObjModel
{
	public:
		tinyObjModel();
		tinyObjModel(const std::string& path);
		~tinyObjModel();

		void loadModel(const std::string& path);
		void draw();
	private:
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		GLuint VAO;
		GLuint VBO;
		GLuint EBO;
		unsigned int drawCount;

};

