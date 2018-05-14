#ifndef MESH_H
#define MESH_H

#define GLEW_STATIC

enum Location {
	POSITION = 0, 
	NORMAL, 
	TEX_COORD, 
	TANGENT,
	BITANGENT,
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
using glm::vec2;
using std::map;
using std::stringstream;

#define INVALID_MATERIAL 0xFFFFFFFF
struct BoneInfo {
	aiMatrix4x4 BoneOffset;
	aiMatrix4x4 FinalTransformation;
	BoneInfo() {
		SetZero(&BoneOffset);
		SetZero(&FinalTransformation);
	}
};
struct GameTexture {
	uint id;
	string type;
	string path;
};
struct Vertex {
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
	uint IDs[NUM_BONES_PER_VEREX];
	float Weights[NUM_BONES_PER_VEREX];
	void AddBoneData(uint BoneID, float Weight);
};
struct MeshEntry {
	MeshEntry()
	{
		NumIndices = 0;
		BaseVertex = 0;
		BaseIndex = 0;
		MaterialIndex = INVALID_MATERIAL;
	}

	uint NumIndices;
	uint BaseVertex;
	uint BaseIndex;
	uint MaterialIndex;
};

class Mesh {
public:
	vector<Vertex> vertices_;
	vector<uint> indices_;
	vector<GameTexture> textures_;

	virtual void SetupMesh();
	virtual void Draw(const GameShader& shader);
	explicit Mesh(vector<Vertex>& vertices, vector<uint>& indices, vector<GameTexture>& textures)
		: vertices_(vertices), indices_(indices), textures_(textures) { }
	~Mesh();
	virtual void BoneTransform(float TimeInSeconds, vector<aiMatrix4x4>& Transforms, aiAnimation* pAnimation,
		const aiScene* scene, aiMatrix4x4& GlobalInverseTransform) { }

protected:
	uint VBO, EBO, VAO;
};

class AnimatedMesh : public Mesh {
public: 
	vector<BoneInfo> BoneInfo_;
	uint NumBones_;
	map<string, uint> BoneMapping_;

	void SetupMesh() override;
	void Draw(const GameShader& shader) override;
	AnimatedMesh(vector<Vertex>& vertices, vector<uint>& indices, vector<GameTexture>& textures, vector<BoneInfo>& BonesInfo, 
		uint& NumBones, map<string, uint>& BoneMapping)
		: Mesh(vertices, indices, textures), BoneInfo_(BonesInfo), NumBones_(NumBones), BoneMapping_(BoneMapping) { }

	uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string& NodeName);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform,
		aiAnimation* pAnimation, aiMatrix4x4& GlobalInverseTransform);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void BoneTransform(float TimeInSeconds, vector<aiMatrix4x4>& Transforms, aiAnimation* pAnimation,
		const aiScene* scene, aiMatrix4x4& GlobalInverseTransform);

protected:
	uint BONE_VB;
};

Mesh* CreateMesh(vector<Vertex>& vertices, vector<uint>& indices, vector<GameTexture>& textures);
Mesh* CreateAnimatedMesh(vector<Vertex>& vertices, vector<uint>& indices, vector<GameTexture>& textures,
	vector<BoneInfo>& BonesInfo, uint& NumBones, map<string, uint>& BoneMapping);

#endif