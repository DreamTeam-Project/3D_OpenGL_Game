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

#include "Physics.h"

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
	explicit GameModel( phys_world& real_world_, int& type, vec3& place, vec3& quat, string& path_2, vec3& scale, double mass, vec3& box, float shininess = 32.0f, bool draw = true) :
		draw_(draw),
		shininess_(shininess),
		path_(path_2),
		quat_(quat),
		scale_(scale),
		type_(type) 
	{
		if (type == 1) {
			rigid_body_ = new phys_body(real_world_, btVector3(place.x, place.y, place.z), btVector3(box.x, box.y, box.z), btScalar(mass));
		}
		if (type == 2) {
			rigid_body_ = new Character(real_world_, btVector3(place.x, place.y, place.z), btVector3(box.x, box.y, box.z), btScalar(mass));
		}

	}
	void Draw(const GameShader& shader);
	void LoadModel();
	void CopyModel(const GameModel* model);
	void ClearLoaded();

	void SetShaderParameters(const GameShader& shader);
	virtual void Move(mat4& model);
	virtual void PrintModel();

	~GameModel() {
		delete rigid_body_;
	}


	string directory_;
	string path_;
	vec3 quat_;
	vec3 scale_;
	int type_;
	float shininess_;
	bool draw_;

	phys_body* rigid_body_;

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
	Structure(phys_world& real_world_, int& type, vec3& place, vec3& quat, string& path_2, vec3& scale, double mass, vec3& box, float shininess, bool draw = true) :
		GameModel(real_world_, type,  place, quat, path_2, scale, mass,  box, shininess, draw) { }
	//void Move(mat4& model) override;
	void PrintModel() override;
private:
	mat4 model;
};

class StreetLamp : public Structure {
public:
	StreetLamp() = delete;
	StreetLamp(phys_world& real_world_, int& type, vec3& place, vec3& quat, string& path_2, vec3& scale, double mass, vec3& box, float shininess, bool draw = true, bool on = true) :
		Structure( real_world_, type, place, quat,  path_2,  scale,  mass, box, shininess, draw),
		light(on)
	{ }
private:
	bool light;
};

class AnimatedModel : public GameModel {
public:
	AnimatedModel(phys_world& real_world_, int& type, vec3& place, vec3& quat, string& path_2, vec3& scale, double mass, vec3& box, float shininess, bool draw = true) : 
		GameModel( real_world_,  type,  place,  quat, path_2, scale, mass, box,shininess, draw) { }
	void PrintModel() override;
};

#endif