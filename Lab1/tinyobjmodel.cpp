#include "tinyobjmodel.h"
#include "MeshHandler.h"

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyObj/tiny_obj_loader2.h>

#include "windows.h"
#include "psapi.h"

using namespace std;

tinyObjModel:: tinyObjModel()
{
	//ctor
    //loadModel("../res/Models/Honours Models/obj/avocado/avocado.obj");
    //loadModel("../res/Models/ship.obj"); 
    //loadModel("../res/Models/Honours Models/obj/backpacknotex/backpack.obj");
    //loadModel("../res/Models/backpack/backpack.obj");
    loadModel("../res/Models/Honours Models/obj/astro/astro.obj");
}

tinyObjModel:: ~tinyObjModel()
{
	//dtor
}

void tinyObjModel::loadModel(const std::string& path)
{
    Timer t;
    std::cout << "OBJ: Start Loading OBJ file" << path << std::endl;
    //std::string inputfile = "../res/Models/ship.obj"; overide path, quick vertice check
    tinyobj::ObjReaderConfig reader_config;
    //reader_config.mtl_search_path = "../res/Models/Honours Models/obj/avocado/"; // Path to material files, temorary fix

    reader_config.mtl_search_path = "";

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                Vertex vertex;
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                glm::vec3 vector = glm::vec3(vx, vy, vz);
                vertex.pos = vector;
              
                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                    vector = glm::vec3(nx, ny, nz);
                    vertex.normal = vector;
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                    glm::vec2 vec = glm::vec2(tx, ty);
                    vertex.texCoord = vec;
                }
                else
                {
					vertex.texCoord = glm::vec3(0.0f, 0.0f, 0.0f);
                }
                vertices.push_back(vertex);

                // Optional: vertex colors
                // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];

        }

    }
    std::cout << "OBJ: File Loaded" << path << std::endl;
    std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";
    std::cout << "--------------------------------------------" << std::endl;

    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
    std::cout << "Memory used by OBJ Model: " << virtualMemUsedByMe << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

}