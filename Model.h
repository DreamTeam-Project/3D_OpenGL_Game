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

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class GameModel {
public:
	GameModel() = delete;
	explicit GameModel(float shininess = 32.0f, bool draw = true ) : draw_(draw), shininess_(shininess) {	}
	void Draw(const GameShader& shader);
	void LoadModel();
	void CopyModel(const GameModel* model);

	void SetShaderParameters(const GameShader& shader);
	void Move(mat4& model);
	virtual void PrintModel();

	string directory_;
	string path_;
	vec3 place_;
	vec3 quat_;
	vec3 scale_;
	int type_;
	float shininess_;

private:
	vector<GameTexture> textures_loaded_;
	vector<MeshEntry> Entries_;
	vector<Mesh*> meshes_;
	bool draw_;
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
	//void SetShaderParameters(GameShader& shader) override;
	//void Move(mat4& model) override;
	void PrintModel() override;
	void Kill() {	}
};

class AnimatedModel : public GameModel {
public:
	AnimatedModel(float shininess, bool draw = true) : GameModel(shininess, draw) { }
	//void SetShaderParameters(GameShader& shader) override;
	//void Move(mat4& model) override;
	void PrintModel() override;
	void Kill() {	}
};

#endif