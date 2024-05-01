#pragma once
#include <glm\glm.hpp>
#include <GL\glew.h>
#include <string>
#include "obj_loader.h"
#include "transform.h"
#include "ShaderHandler.h"
#include "Clock.h"

// Assimp to load many different file formats for reference. known to be slow in debug mode and may have niche issues with fbx files.
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <tinyObj/tiny_obj_loader2.h>

// Here we are focusing on 1 file format per reader for research purposes. obj reader, fbx sdk, json reader
//#include <json/json.hpp> // TO HELP READ JSON FILES, in this case our glTF files
//using json = nlohmann::json; MOVED TO TINYGLTF LIBRARY WHICH ALREADY USES THIS

#include <tinyglTF/tiny_gltf.h>

#include <fbxsdk.h> // This is the fbx sdk, it has to be installed locally (licencing issues + its 2gb) and is not included in the project code files
#if _DEBUG //  The required libraries for the fbx sdk, linking them here as it's easier than adding them to the project settings / good for portability in the future.
#pragma comment(lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.2\\lib\\vs2019\\x64\\debug\\libfbxsdk-md.lib")
#pragma comment(lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.2\\lib\\vs2019\\x64\\debug\\libxml2-md.lib")
#pragma comment(lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.2\\lib\\vs2019\\x64\\debug\\zlib-md.lib")
#else
#pragma comment(lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.2\\lib\\vs2019\\x64\\release\\libfbxsdk-md.lib")
#pragma comment(lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.2\\lib\\vs2019\\x64\\release\\libxml2-md.lib")
#pragma comment(lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.2\\lib\\vs2019\\x64\\release\\zlib-md.lib")
#endif

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
	glm::vec3 Tangent;
	//// bitangent
	glm::vec3 Bitangent;
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

#pragma region GLTFModel 
struct VaoRange
{
	GLsizei start; // Index of first element in vertexArrayObjects
	GLsizei count; // Number of elements in range
};

class GlTFModel // Ideally this would be in the Model class, read the file extension and use the correct reader for the file type. keeping it separate to compare with the other readers at the moment.
{ // New class just for gltf / glb due to unique loading requirements, created with help from https://gltf-viewer-tutorial.gitlab.io/initialization/
public:
	GlTFModel();
	GlTFModel(const std::string &path);

	bool loadGltfFile(const std::string& path, int type);

	std::vector<GLuint> createBufferObjects(); // const tinygltf::Model& model
	std::vector<GLuint> createVertexArrayObjects();

	void Draw();

private:

	//std::filesystem::path _file;
	tinygltf::Model model; // tinygltf model definition
	std::vector<GLuint> bufferObjects;
	std::vector<VaoRange> meshToVertexArrays;
	std::vector<GLuint> vertexArrayObjects;
};
#pragma endregion

#pragma region FBXModel
struct scene;
struct scene_data;
struct mesh;
struct geo_import_settings;

class fbxDescriptor // this is a descriptor for the fbx model, it will be used to store the data from the fbx model
{
public:
	fbxDescriptor(const char* file, scene* scene, scene_data* data) : _scene{ scene }, _data{ data }
	{
		assert(file && _scene && _data); 
		if (fbx_init())
		{
			loadFbxFile(file);
			assert(is_valid());
		}
	}

	~fbxDescriptor()
	{
		_fbxScene->Destroy();
		_manager->Destroy();
		//ZeroMemory(this, sizeof(fbxDescriptor));
	}

	constexpr bool is_valid() const { return _manager && _scene; } // check if the fbx model is valid
	constexpr glm::f32 scale() const { return _scaleFactor; } // get the scale factor of the fbx model
private:
	bool fbx_init();
	void loadFbxFile(const char* file);

	scene* _scene{nullptr};
	scene_data* _data{nullptr};
	FbxManager* _manager{nullptr};
	FbxScene* _fbxScene{nullptr};
	glm::f32 _scaleFactor{1.0f};
	
};


class FBXModel
{
public:
	FBXModel();
	FBXModel(const std::string& filename);
	~FBXModel();

	void loadFbxModel(const std::string& filename);


private:
	FbxManager* _manager{ nullptr };
	FbxScene* _fbxScene{ nullptr };
	FbxImporter* _importer{ nullptr };
	FbxIOSettings* _ios{ nullptr };
	FbxGeometryConverter* _converter{ nullptr };
	FbxNode* _rootNode{ nullptr };
	

};
#pragma endregion


#pragma region oldcode
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
#pragma endregion