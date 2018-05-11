#ifndef MODEL_H
#define MODEL_H

#define GLEW_STATIC

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image_aug.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/anim.h>

#include "Mesh.h"
#include "Shader.h"
#include "System.h"

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
using glm::vec2;
using glm::mat4;

#define ASSIMP_FLAGS aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs
uint TextureFromFile(const char *path, const string &directory);

class GameModel {
public:
	GameModel() = delete;
	explicit GameModel(const GameModel* model, vec3 place, vec3 quat, vec3 scale, bool draw = false);
	explicit GameModel(float shininess = 32.0f, bool draw = true ) : draw_(draw), shininess_(shininess) {	}
	void Draw(const GameShader& shader);
	void LoadModel();
	void CopyModel(const GameModel* model);
	void ClearLoaded();

	void SetShaderParameters(const GameShader& shader);
	virtual void Move(mat4& model);
	virtual void PrintModel();

	string directory_;
	string path_;
	vec3 place_;
	vec3 quat_;
	vec3 scale_;
	int type_;
	float shininess_;
	bool draw_;

private:
	vector<GameTexture> textures_loaded_;
	vector<MeshEntry> Entries_;
	vector<Mesh*> meshes_;
	aiMatrix4x4 GlobalInverseTransform_;
	const aiScene* scene_;

	void ProcessNode(aiNode *node, const aiScene *scene);
	Mesh* ProcessMesh(aiMesh *mesh, const aiScene *scene, uint MeshIndex);
	vector<GameTexture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, const string& typeName);

	void LoadBones(uint MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones,
		vector<BoneInfo>& BonesInfo, map<string, uint>& BoneMapping, uint& NumBones);
};

class Structure : public GameModel {
public:
	Structure() = delete;
	Structure(float shininess, bool draw = true) : GameModel(shininess, draw) { }
	//void Move(mat4& model) override;
	void PrintModel() override;
private:
	mat4 model;
};

class StreetLamp : public Structure {
public:
	StreetLamp() = delete;
	StreetLamp(float shininess, bool draw = true, bool on = true) : Structure(shininess, draw), light(on) { }
private:
	bool light;
};

class AnimatedModel : public GameModel {
public:
	AnimatedModel(float shininess, bool draw = true) : GameModel(shininess, draw) { }
	void PrintModel() override;
};

#endif