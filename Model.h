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
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "Mesh.h"
#include "Shader.h"
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
	explicit GameModel(const GameModel* model, const vec3& place, const vec3& quat, const vec3& scale, bool draw = false);
	explicit GameModel(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path, 
		const vec3& scale, const double& mass, const vec3& box, float shininess = 32.0f, bool draw = true);
	void Draw(const GameShader& shader);
	void LoadModel();
	void CopyModel(const GameModel* model);
	void ClearLoaded();

	void SetShaderParameters(const GameShader& shader);
	virtual void Move(mat4& model);

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
	vector<Mesh*> meshes_;
	const aiScene* scene_;

	void ProcessNode(aiNode *node, const aiScene *scene);
	Mesh* ProcessMesh(aiMesh *mesh, const aiScene *scene, uint MeshIndex);
	vector<GameTexture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, const string& typeName);
};

class Structure : public GameModel {
public:
	Structure() = delete;
	Structure(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path,
		const vec3& scale, const double& mass, const vec3& box, float shininess, bool draw = true);
	void Move(mat4& model) override;
private:
	mat4 model_;
};

class StreetLamp : public Structure {
public:
	StreetLamp() = delete;
	StreetLamp(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path, 
		const vec3& scale, const double& mass, const vec3& box, float shininess, bool draw = true, bool on = true) :
		Structure( real_world_, type, place, quat,  path,  scale,  mass, box, shininess, draw),
		light(on)
	{ }
private:
	bool light;
};

class AnimatedModel : public GameModel {
public:
	AnimatedModel(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path,
		const vec3& scale, const double& mass, const vec3& box, float shininess, bool draw = true) : 
		GameModel( real_world_,  type,  place,  quat, path, scale, mass, box,shininess, draw) { }
};

#endif