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
using glm::mat4;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class GameModel { // type 0 or default
public:
	struct VertexBoneData {
		uint IDs[NUM_BONES_PER_VEREX];
		float Weights[NUM_BONES_PER_VEREX];
		void AddBoneData(uint BoneID, float Weight);
	};
	struct BoneInfo {
		aiMatrix4x4 BoneOffset;
		aiMatrix4x4 FinalTransformation;
		BoneInfo() {
			SetZero(&BoneOffset);
			SetZero(&FinalTransformation);
		}
	};

	GameModel(bool gamma = false ) : gammaCorrection_(gamma) {	}
	void Draw(const GameShader& shader);
	void LoadModel();

	virtual void Move(mat4& model);
	virtual void PrintModel();

	void BoneTransform(float TimeInSeconds, vector<aiMatrix4x4>& Transforms);

	string directory_;
	string path_;
	vec3 place_;
	vec3 quat_;
	vec3 scale_;
	int type_;

private:
	void ProcessNode(aiNode *node, const aiScene *scene);
	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, const string& typeName);

	void LoadBones(uint MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones);
	uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

	map<string, uint> BoneMapping_;
	uint NumBones_;
	vector<BoneInfo> BoneInfo_;
	aiMatrix4x4 GlobalInverseTransform_;
	const aiScene* scene_;
	Assimp::Importer importer_;
	vector<Texture> textures_loaded_;
	vector<Mesh> meshes_;
	bool gammaCorrection_;
};

class Structure : public GameModel { // type 3
public:
	void Move(mat4& model) override;
	void PrintModel() override;
	void Kill() {	}
};

#endif