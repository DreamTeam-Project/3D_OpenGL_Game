#ifndef MESH_H
#define MESH_H

#define GLEW_STATIC

enum Location {
	POSITION = 0, 
	NORMAL, 
	TEX_COORD, 
	BONE_ID, 
	BONE_WEIGHT
};

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "System.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/anim.h>
#include <assimp/matrix4x4.h>

using std::vector;
using std::map;
using std::stringstream;

#define INVALID_MATERIAL 0xFFFFFFFF
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
struct GameTexture {
	unsigned int id;
	string type;
	string path;
};
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	void addvec(const vec3& a);
};
struct MeshEntry {
	MeshEntry()
	{
		NumIndices = 0;
		BaseVertex = 0;
		BaseIndex = 0;
		MaterialIndex = INVALID_MATERIAL;
	}

	unsigned int NumIndices;
	unsigned int BaseVertex;
	unsigned int BaseIndex;
	unsigned int MaterialIndex;
};

class Mesh {
public:
	vector<Vertex> vertices_;
	vector<unsigned int> indices_;
	vector<GameTexture> textures_;

	virtual void Draw(const GameShader& shader);
	friend Mesh* CreateMesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<GameTexture>& textures);
	virtual void SetupMesh();
	Mesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<GameTexture>& textures)
		: vertices_(vertices), indices_(indices), textures_(textures) { }
protected:
	unsigned int VBO, EBO, VAO;
};

class AnimatedMesh : public Mesh {
public: 
	vector<BoneInfo> BoneInfo_;
	vector<VertexBoneData> Bones_;
	uint NumBones_;
	map<string, uint> BoneMapping_;

	void SetupMesh() override;
	void Draw(const GameShader& shader) override;
	friend Mesh* CreateAnimatedMesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<GameTexture>& textures, 
		vector<BoneInfo>& BonesInfo, vector<VertexBoneData>& Bones, uint& NumBones, map<string, uint>& BoneMapping);

	uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform,
		aiAnimation* pAnimation, aiMatrix4x4& GlobalInverseTransform);
	void BoneTransform(float TimeInSeconds, vector<aiMatrix4x4>& Transforms, aiAnimation* pAnimation,
		aiScene* scene, aiMatrix4x4& GlobalInverseTransform);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

	AnimatedMesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<GameTexture>& textures, vector<BoneInfo>& BonesInfo,
		vector<VertexBoneData>& Bones, uint& NumBones, map<string, uint>& BoneMapping)
		: Mesh(vertices, indices, textures), BoneInfo_(BonesInfo), Bones_(Bones), NumBones_(NumBones), BoneMapping_(BoneMapping) { }

private:
	unsigned int BONE_VD;
};

#endif