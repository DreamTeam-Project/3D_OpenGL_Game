#define GLEW_STATIC

#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image_aug.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include "Types.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using std::map;
using std::vector;
using std::string;
using glm::vec3;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model_t {
public:
	vector<Texture> textures_loaded_c;
	vector<Mesh_t> meshes_c;
	string directory_c;
	bool gammaCorrection_c;
	Model_t(Map_t* objects, bool gamma = false) : gammaCorrection_c(gamma) { LoadModels(objects); }
	void Draw(Shader_t shader);
	void ReplaceModel(vec3 replaceTo, size_t model);

private:
	void LoadModels(Map_t* objects);
	void ProcessNode(aiNode *node, const aiScene *scene, vec3 place);
	Mesh_t ProcessMesh(aiMesh *mesh, const aiScene *scene, vec3 place);
	vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

#endif