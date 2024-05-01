#include "MeshHandler.h"
#include <vector>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tinyglTF/tiny_gltf.h>

#include "windows.h"
#include "psapi.h"
//actually lost as to why including these is not what is needed, but the error is gone and it works.
//#include "stb_image.h" 
//#include "stb_image_write.h"

#pragma region Mesh
void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex texture coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	// vertex normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	/*
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
	*/

	glBindVertexArray(0);
}

void Mesh::Draw(const ShaderHandler& shader)
{
	/* This is the way learnopengl.com does it, to get the textures along side the model loading process, would be good to get this working
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	//unsigned int normalNr = 0, heightNr = 0, ambientNr = 0;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);

		shader.setInt(("material." + name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	*/
	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//glActiveTexture(GL_TEXTURE0); //back to default once configured.
}
#pragma endregion

#pragma region Model

Model::Model()
{

}
void Model::Draw(const ShaderHandler& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i]->Draw(shader);
}

void Model::clearModel()
{
	meshes.clear();
	textures_loaded.clear();
}


void Model::loadModel(std::string const &path)
{
	Timer t;
	std::cout << "ASSIMP:: Loading Model " << path << std::endl;
	Assimp::Importer import;
	const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate  | aiProcess_JoinIdenticalVertices);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
	std::cout << "ASSIMP:: Loaded Model " << path << std::endl;
	std::cout << "Total Time elapsed: " << t.elapsed() << " seconds\n";
	std::cout << "--------------------------------------------" << std::endl;
}


void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene); //FALLBACK, trying to get node transformations
	}
}

std::shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordinates
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.pos = vector;
		if (mesh->HasNormals())
		{
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;
		}
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoord = vec;
		}
		else
			vertex.texCoord = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);

	}
	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
		// process material
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}
	//return Mesh(vertices, indices, textures);
	return std::shared_ptr<Mesh> (new Mesh(move(vertices), move(indices), move(textures)));
}

//std::shared_ptr<Texture>
vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture); // add to loaded textures
		}
	}
	return textures;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

#pragma endregion

// I would move these to their own separate files, but for some reason it breaks something else im not even using below. 
#pragma region GLTFModel 

GlTFModel::GlTFModel()
{
	std::cout << "TEMP GLTF" << std::endl;
}

GlTFModel::GlTFModel(std::string const& path)
{
	std::cout << "TEMP GLTF LOAD ATTEMPT 1" << std::endl;
	loadGltfFile(path,0);
}

bool GlTFModel::loadGltfFile(std::string const& path, int type)
{
	Timer t;
	std::cout << "GLTF: Start Loading glTF file" << path << std::endl;
	// Load the model
	std::string err;
	std::string warn;
	tinygltf::TinyGLTF loader;

	bool ret = 0;
	if (type == 0)
	{
		ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
	}
	else if (type == 1)
	{
		ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);  // for binary glTF(.glb)
	}

	if (!warn.empty()) {
		std::cout << "GLTF: Warn: " << warn << std::endl;
		//return false;
	}
	if (!err.empty()) {
		std::cout << "GLTF: Err: " << err << std::endl;
		//return false;
	}
	if (!ret) {
		std::cout << "Failed to parse glTF file" << std::endl;
		return false;
	}
	// Load the textures
	//loadTextures();
	// Load the meshes
	//loadMeshes();
	std::cout << "GLTF: File Loaded" << path << std::endl;
	std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";
	std::cout << "--------------------------------------------" << std::endl;
	return true;
}

// We do the following over using the general mesh class that would typically setup 
// buffers and VAOs, as glTF already contains pre-defined buffers that can be directly loaded into the GPU (within OpenGL)
std::vector<GLuint> GlTFModel::createBufferObjects()  
{
	Timer t;
	std::cout << " GLTF: Creating Buffer Objects" << std::endl;
	std::vector<GLuint> bufferObjects(model.buffers.size(), 0);

	glGenBuffers(GLsizei(model.buffers.size()), bufferObjects.data());
	for (size_t i = 0; i < model.buffers.size(); ++i) 
	{
		glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[i]);
		glBufferStorage(GL_ARRAY_BUFFER, model.buffers[i].data.size(), model.buffers[i].data.data(), 0);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	std::cout << " GLTF: Buffer Objects Created" << std::endl;
	std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";
	return bufferObjects;
}

std::vector<GLuint> GlTFModel::createVertexArrayObjects()
{
	Timer t;
	std::cout << " GLTF: Creating Vertex Array Objects" << std::endl;

	meshToVertexArrays.resize(model.meshes.size());

	// Helps to remind of the layout of the vertex attributes, don't want to mix up normal and texcoord again as I was manually putting in numbers before
	const GLuint VERTEX_POSITION_I = 0;
	const GLuint VERTEX_NORMAL_I = 2; // may need to swap normal and texcoord0
	const GLuint VERTEX_TEXCOORD0_I = 1;

	for (size_t i = 0; i < model.meshes.size(); ++i)
	{
		//std::cout << " GLTF: VAO LOOP 1" << std::endl;
		const tinygltf::Mesh& mesh = model.meshes[i];

		VaoRange& vaoRange = meshToVertexArrays[i];
		vaoRange.start = GLsizei(vertexArrayObjects.size());
		vaoRange.count = GLsizei(mesh.primitives.size());

		vertexArrayObjects.resize(vertexArrayObjects.size() + mesh.primitives.size());

		//glGenVertexArrays(GLsizei(mesh.primitives.size()), vertexArrayObjects.data() + vaoRange.start);
		glGenVertexArrays(vaoRange.count, &vertexArrayObjects[vaoRange.start]);

		for (size_t j = 0; j < mesh.primitives.size(); ++j)
		{
			//std::cout << " GLTF: VAO LOOP 2" << std::endl;
			const auto vao = vertexArrayObjects[vaoRange.start + j];
			const tinygltf::Primitive& primitive = mesh.primitives[j];
			glBindVertexArray(vao);
			// Position
			{ // opening a scope allows you to use the same variable names in different blocks, useful for reusing variable names, below is a good example where the accessor looking for position,will be changed to normal in the next scope
				//std::cout << " GLTF: POS" << std::endl;
				const auto iterator = primitive.attributes.find("POSITION");
				if (iterator != end(primitive.attributes)) { // If "POSITION" has been found in the map
				// (*iterator).first is the key "POSITION", (*iterator).second is the value, ie. the index of the accessor for this attribute
					const auto accessorIdx = (*iterator).second;
					const auto& accessor = model.accessors[accessorIdx];
					const auto& bufferView = model.bufferViews[accessor.bufferView];
					const auto bufferIdx = bufferView.buffer;
			
					glEnableVertexAttribArray(VERTEX_POSITION_I);

					assert(GL_ARRAY_BUFFER == bufferView.target);
					glBindBuffer(GL_ARRAY_BUFFER, bufferView.target); // ERROR HERE using bufferObjects[bufferIdx], changed to bufferView.target

					// "tinygltf converts strings type like "VEC3, "VEC2" to the number of components, stored in accessor.type"
					const auto byteOffset = accessor.byteOffset + bufferView.byteOffset;
					glVertexAttribPointer(VERTEX_POSITION_I, accessor.type, accessor.componentType, GL_FALSE, GLsizei(bufferView.byteStride), (const GLvoid*)byteOffset);
			}
			// Normal, repeated code but for normals
			{
				//std::cout << " GLTF: NORMAL" << std::endl;
				const auto iterator = primitive.attributes.find("NORMAL");
				if (iterator != end(primitive.attributes)) {
					const auto accessorIdx = (*iterator).second;
					const auto& accessor = model.accessors[accessorIdx];
					const auto& bufferView = model.bufferViews[accessor.bufferView];
					const auto bufferIdx = bufferView.buffer;

					glEnableVertexAttribArray(VERTEX_NORMAL_I);

					assert(GL_ARRAY_BUFFER == bufferView.target);
					glBindBuffer(GL_ARRAY_BUFFER, bufferView.target);
					// no need to get the byteOffset again, as it is the same as the position
					glVertexAttribPointer(VERTEX_NORMAL_I, accessor.type, accessor.componentType, GL_FALSE, GLsizei(bufferView.byteStride), (const GLvoid*)(accessor.byteOffset + bufferView.byteOffset));
				}
			}
			// Texcoord0, repeated code but for Texcoord0
			{
				//std::cout << " GLTF: TEXCOORD" << std::endl;
				const auto iterator = primitive.attributes.find("TEXCOORD_0");
				if (iterator != end(primitive.attributes))
				{
					const auto accessorIdx = (*iterator).second;
					const auto& accessor = model.accessors[accessorIdx];
					const auto& bufferView = model.bufferViews[accessor.bufferView];
					const auto bufferIdx = bufferView.buffer;

					glEnableVertexAttribArray(VERTEX_TEXCOORD0_I);

					assert(GL_ARRAY_BUFFER == bufferView.target);
					glBindBuffer(GL_ARRAY_BUFFER, bufferView.target);
					// no need to get the byteOffset again, as it is the same as the position
					glVertexAttribPointer(VERTEX_TEXCOORD0_I, accessor.type, accessor.componentType, GL_FALSE, GLsizei(bufferView.byteStride), (const GLvoid*)(accessor.byteOffset + bufferView.byteOffset));
				}
			}
		}
		// index
			if (primitive.indices >= 0) 
			{
				//std::cout << " GLTF: INDEX" << std::endl;
				const auto accessorIdx = primitive.indices;
				const auto& accessor = model.accessors[accessorIdx];
				const auto& bufferView = model.bufferViews[accessor.bufferView];
				const auto bufferIdx = bufferView.buffer;

				assert(GL_ELEMENT_ARRAY_BUFFER == bufferView.target);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferView.target);
			}
		}
	}
	glBindVertexArray(0); // unbind the VAO
	std::clog << "GLTF: Number of VAOs: " << vertexArrayObjects.size() << std::endl; // print the number of VAOs created
	std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";

	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
	std::cout << "Memory used by GLTF Model: " << virtualMemUsedByMe << std::endl;
	std::cout << "--------------------------------------------" << std::endl;
	return vertexArrayObjects; // finally return the VAOs
};

std::vector<GLuint> GlTFModel::createTextureObjects()
{
	Timer t;
	std::cout << " GLTF: Creating Texture Objects" << std::endl;
	std::vector<GLuint> textureObjects(model.textures.size(), 0);

	// default sampler:
	// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#texturesampler
	// "When undefined, a sampler with repeat wrapping and auto filtering should
	// be used."
	tinygltf::Sampler defaultSampler;
	defaultSampler.minFilter = GL_LINEAR;
	defaultSampler.magFilter = GL_LINEAR;
	defaultSampler.wrapS = GL_REPEAT;
	defaultSampler.wrapT = GL_REPEAT;
	//defaultSampler.wrapR = GL_REPEAT;

	glActiveTexture(GL_TEXTURE0);

	glGenTextures(GLsizei(model.textures.size()), textureObjects.data());
	for (size_t i = 0; i < model.textures.size(); ++i) {
		const auto& texture = model.textures[i];
		assert(texture.source >= 0);
		const auto& image = model.images[texture.source];

		const auto& sampler =
			texture.sampler >= 0 ? model.samplers[texture.sampler] : defaultSampler;
		glBindTexture(GL_TEXTURE_2D, textureObjects[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
			GL_RGBA, image.pixel_type, image.image.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			sampler.minFilter != -1 ? sampler.minFilter : GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			sampler.magFilter != -1 ? sampler.magFilter : GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, sampler.wrapR);

		if (sampler.minFilter == GL_NEAREST_MIPMAP_NEAREST ||
			sampler.minFilter == GL_NEAREST_MIPMAP_LINEAR ||
			sampler.minFilter == GL_LINEAR_MIPMAP_NEAREST ||
			sampler.minFilter == GL_LINEAR_MIPMAP_LINEAR) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	std::cout << " GLTF: Texture Objects Created" << std::endl;
	std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";
	std::cout << "--------------------------------------------" << std::endl;

	return textureObjects;

}

void GlTFModel::Draw()
{
	//std::cout << " GLTF: DRAW" << std::endl;
	const std::function<void(int, const glm::mat4&)> drawNode = [&](int nodeIdx, const glm::mat4& parentMatrix) {
		// TODO The drawNode function
		const auto& node = model.nodes[nodeIdx];
		//const glm::mat4 modelMatrix = getLocalToWorldMatrix(node, parentMatrix);
		const glm::mat4 modelMatrix = glm::mat4(1); // TEMP
		// If the node references a mesh (a node can also reference a camera, or a light)
		if (node.mesh >= 0) {
			//auto mvMatrix = viewMatrix * modelMatrix; // Also called localToCamera matrix
			//const auto mvpMatrix = projMatrix * mvMatrix; // Also called localToScreen matrix

			// Normal matrix is necessary to maintain normal vectors
			// orthogonal to tangent vectors
			// https://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/

			//const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

			//glUniformMatrix4fv(modelViewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
			//glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
			//glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

			const auto& mesh = model.meshes[node.mesh];
			const auto& vaoRange = meshToVertexArrays[node.mesh];
			for (size_t pIdx = 0; pIdx < mesh.primitives.size(); ++pIdx) {
				const auto vao = vertexArrayObjects[vaoRange.start + pIdx];
				const auto& primitive = mesh.primitives[pIdx];
				glBindVertexArray(vao);
				if (primitive.indices >= 0) {
					const auto& accessor = model.accessors[primitive.indices];
					const auto& bufferView = model.bufferViews[accessor.bufferView];
					const auto byteOffset = accessor.byteOffset + bufferView.byteOffset;
					glDrawElements(primitive.mode, GLsizei(accessor.count), accessor.componentType, (const GLvoid*)byteOffset);
				}
				else {
					// Take first accessor to get the count
					const auto accessorIdx = (*begin(primitive.attributes)).second;
					const auto& accessor = model.accessors[accessorIdx];
					glDrawArrays(primitive.mode, 0, GLsizei(accessor.count));
				}
			}
		}
		// Draw children
		for (const auto childNodeIdx : node.children) {
			drawNode(childNodeIdx, modelMatrix);
		}
	};

	// Draw the scene referenced by gltf file
	if (model.defaultScene >= 0) {
		// TODO Draw all nodes
		for (const auto nodeIdx : model.scenes[model.defaultScene].nodes) {
			drawNode(nodeIdx, glm::mat4(1));
		}
	}
};

#pragma endregion

#pragma region FBXModel 

FBXModel::FBXModel()
{
	// Load the model
	//loadFbxModel("../res/Models/Honours Models/fbx/avocado.fbx");
	//loadFbxModel("../res/Models/Honours Models/fbx/Sci-Fi soldier/source/Idle.fbx");
	loadFbxModel("../res/Models/Honours Models/fbx/BarramundiFish.fbx");
}

FBXModel::FBXModel(std::string const& path)
{
	// Load the model
	loadFbxModel(path);
}

FBXModel::~FBXModel()
{
	_fbxScene->Destroy();
	_manager->Destroy();
}

void FBXModel::loadFbxModel(std::string const& path)
{
	Timer t;
	std::cout << "FBX: Start Loading FBX file" << path << std::endl;

	// Load the model
	_manager = FbxManager::Create();

	FbxIOSettings* ios = FbxIOSettings::Create(_manager, IOSROOT);
	_manager->SetIOSettings(ios);

	_importer = FbxImporter::Create(_manager, "");
	
	bool ret = _importer->Initialize(path.c_str(), -1, _manager->GetIOSettings());
	if (!ret) {
		std::cerr << "FBX: Call to FbxImporter::Initialize() failed.\n";
		std::cerr << "FBX: Error returned: " << _importer->GetStatus().GetErrorString() << "\n\n";
		return;
	}
	_fbxScene = FbxScene::Create(_manager, "myScene");


	//get mesh
	_importer->Import(_fbxScene);

	if (_fbxScene->GetRootNode())
	{
		SetupNode(_fbxScene->GetRootNode(), "null");
	}

	_importer->Destroy();

	std::cout << "FBX: File Loaded" << path << std::endl;
	std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";
	

	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
	std::cout << "Memory used by FBX Model: " << virtualMemUsedByMe << std::endl;

	std::cout << "--------------------------------------------" << std::endl;
}

void FBXModel::SetupNode(FbxNode* pNode, std::string parentName)
{

	if (pNode->GetNodeAttribute() == NULL)
	{
		std::cout << "NULL Node Attribute\n";
	}
	else
	{
		attributeType = (pNode->GetNodeAttribute()->GetAttributeType());
	}

	if (attributeType == FbxNodeAttribute::eMesh)
	{
		FbxMesh* mesh = pNode->GetMesh();
		SetupMesh(mesh);
	}
	//else if (attributeType == FbxNodeAttribute::eSkeleton)
	//{
	// // SCRAPPED FEATURE
	//	FbxSkeleton* skeleton = pNode->GetSkeleton();
	//	//SetupSkeleton(skeleton);
	//}
	//else if (attributeType == FbxNodeAttribute::eNull)
	//{
	//	std::cout << "NULL Node\n";
	//}
	//else if (attributeType == FbxNodeAttribute::eMarker)
	//{
	//	std::cout << "Marker Node\n";
	//}
	//else if (attributeType == FbxNodeAttribute::eCamera)
	//{
	//	std::cout << "Camera Node\n";
	//}
	//else if (attributeType == FbxNodeAttribute::eLight)
	//{
	//	std::cout << "Light Node\n";
	//}
	else
	{
		std::cout << "Unknown Node Attribute\n";
	}
	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		SetupNode(pNode->GetChild(i), pNode->GetName());
	}
}

void FBXModel::SetupMesh(FbxMesh* pMesh)
{
	std::cout << "FBX: Mesh " << pMesh->GetName() << std::endl;
	// Vertices
	FbxVector4* vertices = pMesh->GetControlPoints();
	for (int i = 0; i < pMesh->GetControlPointsCount(); i++)
	{
		std::cout << "Vertex " << i << ": " << vertices[i][0] << " " << vertices[i][1] << " " << vertices[i][2] << std::endl;
	}
	// Indices
	for (int i = 0; i < pMesh->GetPolygonCount(); i++)
	{
		int numVertices = pMesh->GetPolygonSize(i);
		for (int j = 0; j < numVertices; j++)
		{
			int vertexIndex = pMesh->GetPolygonVertex(i, j);
			std::cout << "Index" << vertexIndex << std::endl;
		}
	}

	// Normals
	FbxGeometryElementNormal* normalElement = pMesh->GetElementNormal();
	if (normalElement)
	{
		for (int i = 0; i < pMesh->GetControlPointsCount(); i++)
		{
			FbxVector4 normal = normalElement->GetDirectArray().GetAt(i);
			std::cout << "Normal " << i << ": " << normal[0] << " " << normal[1] << " " << normal[2] << std::endl;
		}
	}

	// UVs
	FbxGeometryElementUV* uvElement = pMesh->GetElementUV();
	if (uvElement)
	{
		for (int i = 0; i < pMesh->GetControlPointsCount(); i++)
		{
			FbxVector2 uv = uvElement->GetDirectArray().GetAt(i);
			std::cout << "UV " << i << ": " << uv[0] << " " << uv[1] << std::endl;
		}
	}

	// Materials
	FbxNode* node = pMesh->GetNode();
	if (node)
	{
		int materialCount = node->GetMaterialCount();
		for (int i = 0; i < materialCount; i++)
		{
			FbxSurfaceMaterial* material = node->GetMaterial(i);
			std::cout << "Material " << i << ": " << material->GetName() << std::endl;
		}
	}

	// Textures
	FbxProperty property = pMesh->GetFirstProperty();
	while (property.IsValid())
	{
		if (property.GetSrcObjectCount<FbxTexture>() > 0)
		{
			FbxTexture* texture = property.GetSrcObject<FbxTexture>();
			std::cout << "Texture: " << texture->GetName() << std::endl;
		}
		property = pMesh->GetNextProperty(property);
	}
	// Skinning
	FbxSkin* skin = (FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin);
	if (skin)
	{
		std::cout << "Skinning" << std::endl;
		int clusterCount = skin->GetClusterCount();
		for (int i = 0; i < clusterCount; i++)
		{
			FbxCluster* cluster = skin->GetCluster(i);
			FbxNode* link = cluster->GetLink();
			std::cout << "Cluster " << i << ": " << link->GetName() << std::endl;
		}

	}
};


#pragma endregion


#pragma region oldcode
void MeshHandler::init(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices)
{
	IndexedModel model;

	for (unsigned int i = 0; i < numVertices; i++)
	{
		model.positions.push_back(*vertices[i].GetPos());
		model.texCoords.push_back(*vertices[i].GetTexCoord());
		model.normals.push_back(*vertices[i].GetNormal());
	}

	for (unsigned int i = 0; i < numIndices; i++)
		model.indices.push_back(indices[i]);

	initModel(model);
}

void MeshHandler::initModel(const IndexedModel& model)
{
	drawCount = model.indices.size();

	glGenVertexArrays(1, &VAO); // generate a vertex array and store it in the VAO
	glBindVertexArray(VAO); // bind the VAO (any operation that works on a VAO will work on our bound VAO - binding)

	glGenBuffers(NUM_BUFFERS, VAB); // generate our buffers based of our array of data/buffers - GLuint vertexArrayBuffers[NUM_BUFFERS];

	glBindBuffer(GL_ARRAY_BUFFER, VAB[POSITION_VB]); // tell opengl what type of data the buffer is (GL_ARRAY_BUFFER), and pass the data
	glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.positions[0]), &model.positions[0], GL_STATIC_DRAW); // move the data to the GPU - type of data, size of data, starting address (pointer) of data, where do we store the data on the GPU (determined by type)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VAB[TEXCOORD_VB]); // tell opengl what type of data the buffer is (GL_ARRAY_BUFFER), and pass the data
	glBufferData(GL_ARRAY_BUFFER, model.positions.size() * sizeof(model.texCoords[0]), &model.texCoords[0], GL_STATIC_DRAW); // move the data to the GPU - type of data, size of data, starting address (pointer) of data, where do we store the data on the GPU
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VAB[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(model.normals[0]) * model.normals.size(), &model.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VAB[INDEX_VB]); // tell opengl what type of data the buffer is (GL_ARRAY_BUFFER), and pass the data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(model.indices[0]), &model.indices[0], GL_STATIC_DRAW); // move the data to the GPU - type of data, size of data, starting address (pointer) of data, where do we store the data on the GPU

	glBindVertexArray(0); // unbind our VAO
}

MeshHandler::MeshHandler()
{
	drawCount = NULL;
	VAB[0] = NULL;
	VAB[1] = NULL;
	VAB[2] = NULL;
	VAB[3] = NULL;
	VAO = NULL;
}

MeshHandler::MeshHandler(const std::string& filename)
{
	loadModel(filename);
}

void MeshHandler::loadModel(const std::string& filename)
{
	IndexedModel model = OBJModel(filename).ToIndexedModel();
	initModel(model);
	Sphere meshSphere();
}

MeshHandler::~MeshHandler()
{
	glDeleteVertexArrays(1, &VAO); // delete arrays
}

void MeshHandler::draw()
{
	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, drawCount);

	glBindVertexArray(0);
}

void MeshHandler::updateSphereData(glm::vec3 pos, float radius)
{
	meshSphere.SetPos(pos);
	meshSphere.SetRadius(radius);
}

#pragma endregion

