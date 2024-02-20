#pragma once
#include <glm\glm.hpp>
#include <GL\glew.h>
#include <string>
#include "obj_loader.h"
#include "transform.h"
#include "ShaderHandler.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using std::vector;

struct Vertex
{
public:
	/*
	Vertex(const glm::vec3& pos, const glm::vec2& texCoord)
	{
		this->pos = pos;
		this->texCoord = texCoord;
		this->normal = normal;
	}
	*/
	glm::vec3* GetPos() { return &pos; }
	glm::vec2* GetTexCoord() { return &texCoord; }
	glm::vec3* GetNormal() { return &normal; }


	glm::vec3 pos;
	glm::vec2 texCoord;
	glm::vec3 normal;

	// not to sure if I need these yet
	//// tangent
	//glm::vec3 Tangent;
	//// bitangent
	//glm::vec3 Bitangent;
	////bone indexes which will influence this vertex
	//int m_BoneIDs[4];
	////weights from each bone
	//float m_Weights[4];

private:

};

struct Sphere
{
public:

	Sphere()
	{
		this->pos = glm::vec3(0.0f, 0.0f, 0.0f);
		this->radius = 1.0f;
	}

	glm::vec3 GetPos() { return pos; }
	float GetRadius() { return radius; }

	void SetPos(glm::vec3 pos)
	{
		this->pos = pos;
	}

	void SetRadius(float radius)
	{
		this->radius = radius;
	}

private:
	glm::vec3 pos;
	float radius;
};

// The start of implementing assimp, basically starting from scratch to avoid the issues with the obj loader to
// remove it safely and to learn how to use assimp while trying to improve performance passing by reference. 
// Thanks to the tutorial at http://www.learnopengl.com/#!Model-Loading/Model as the base for this code.
// "Some versions of Assimp tend to load models quite slow when using the debug version and /or the debug mode of your IDE"

struct Texture // since we're using assimp it seems good to follow using a new texture struct to store the data direct from a model load rather than the old method.
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	// mesh data
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	// How I would normally do it but I'm trying to learn how to use move semantics and rvalue references to improve performance
	//Mesh(const vector<Vertex>& vertices, const vector<unsigned int>& indices, const vector<Texture>& textures);

	// Move constructor
	Mesh(vector<Vertex>&& vertices, vector<unsigned int>&& indices, vector<Texture>&& textures):
	vertices{ std::move(vertices) }, indices{ std::move(indices) }, textures{ std::move(textures)}
	{
		setupMesh();
	}
	//
	Mesh(const vector<Vertex>& vertices = vector<Vertex>(0), const vector<unsigned int>& indices = vector<unsigned int>(0), const vector<Texture>& Textures = vector<Texture>(0)) :
		vertices{ vertices }, indices{ indices }, textures{ textures }
	{
		setupMesh();
	}

	Mesh(const Mesh& other) {
		init(other);
	}
	Mesh(Mesh&& other) {
		init(other);
	}

	Mesh& operator=(const Mesh& other)
	{
		if (this == &other)
			return *this;
		return init(other);
	}

	Mesh& operator=(Mesh&& other)
	{
		if (this == &other)
			return *this;
		return init(other);
	}

	void Draw(const ShaderHandler& shader);

private:
	Mesh& init(const Mesh& other) {
		vertices = other.vertices;
		indices = other.indices;
		textures = other.textures;
		VAO = other.VAO;
		VBO = other.VBO;
		EBO = other.EBO;
		return *this;
	}
	Mesh& init(Mesh&& other) {
		vertices = other.vertices;
		indices = other.indices;
		textures = other.textures;
		VAO = other.VAO;
		VBO = other.VBO;
		EBO = other.EBO;
		return *this;
	}

	//  render data
	unsigned int VAO, VBO, EBO;

	void setupMesh();
};

class Model
{
public:
	Model();
	Model(std::string const &path)
	{
		loadModel(path);
	}
	~Model() {clearModel();}

	//move constructor
	Model(Model&& other) {
		init(other);
	}

	//copy constructor
	Model(const Model& other) {
		init(other);
	}

	Model& operator=(Model&& other)
	{
		if (this == &other)
			return *this;
		return init(other);
	}

	Model& operator=(const Model& other)
	{
		if (this == &other)
			return *this;
		return init(other);
	}


	void Draw(const ShaderHandler& shader);
	void loadModel(std::string const& path);
	void clearModel();
private:
	// model data
	//vector<Mesh> meshes;
	vector<std::shared_ptr<Mesh>> meshes;

	vector<Texture> textures_loaded; 
	//vector<std::shared_ptr<Texture>> textures_loaded;

	std::string directory;

	void processNode(aiNode* node, const aiScene* scene);

	//Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);

	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	//vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	unsigned int Model::TextureFromFile(const char* path, const std::string& directory);

	Model& init(const Model& other) {
		meshes = other.meshes;
		textures_loaded = other.textures_loaded;
		directory = other.directory;
		return *this;
	}

	Model& init(Model&& other) {
		meshes = other.meshes;
		textures_loaded = other.textures_loaded;
		directory = other.directory;
		return *this;
	}


};


// OLD
class MeshHandler
{
public:
	MeshHandler();
	MeshHandler(const std::string& filename);
	~MeshHandler();

	void draw();
	void init(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices);
	void loadModel(const std::string& filename);
	void initModel(const IndexedModel& model);
	void updateSphereData(glm::vec3 pos, float radius);
	glm::vec3 getSpherePos() { return meshSphere.GetPos(); }
	float getSphereRadius() { return meshSphere.GetRadius(); }

private:
	enum
	{
		POSITION_VB,
		TEXCOORD_VB,
		NORMAL_VB,
		INDEX_VB,
		NUM_BUFFERS
	};

	Sphere meshSphere;
	GLuint VAO;
	GLuint VAB[NUM_BUFFERS]; // create our array of buffers
	unsigned int drawCount; // how much of the vertexArrayObject do we want to draw
};