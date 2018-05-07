#include "Mesh.h"

//This helper function finds free slots in the VertexBoneData structure and places inside
//the id and bone weight.
void VertexBoneData::AddBoneData(uint BoneID, float Weight)	{
	uint length = sizeof(IDs) / sizeof(IDs[0]);
	for (uint i = 0; i < length; i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}
	throw GameException(__LINE__, __func__, string("Error: to mush bones"));
}

Mesh* CreateMesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<GameTexture>& textures) {
	Mesh* ret = new Mesh(vertices, indices, textures);
	ret->SetupMesh();
	return ret;
}

Mesh* CreateAnimatedMesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<GameTexture>& textures,
	vector<BoneInfo>& BonesInfo, vector<VertexBoneData>& Bones, uint& NumBones, map<string, uint>& BoneMapping) {
	Mesh* ret = new AnimatedMesh(vertices, indices, textures, BonesInfo, Bones, NumBones, BoneMapping);
	ret->SetupMesh();
	return ret;
}

void Mesh::Draw(const GameShader& shader) {
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < textures_.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		string number;
		string name = textures_[i].type;
		if (name == "diffuse") {
			number = to_string(diffuseNr++);
		}
		else if (name == "specular") {
			number = to_string(specularNr++);
		}
		else if (name == "normal") {
			number = to_string(normalNr++);
		}
		else if (name == "height") {
			number = to_string(heightNr++);
		}
		glUniform1i(glGetUniformLocation(shader.program_c, (string("material.") + name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures_[i].id);
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void AnimatedMesh::Draw(const GameShader& shader) {
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < textures_.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		string number;
		string name = textures_[i].type;
		if (name == "texture_diffuse") {
			number = std::to_string(diffuseNr++);
		}
		else if (name == "texture_specular") {
			number = std::to_string(specularNr++);
		}
		else if (name == "texture_normal") {
			number = std::to_string(normalNr++);
		}
		else if (name == "texture_height") {
			number = std::to_string(heightNr++);
		}
		glUniform1i(glGetUniformLocation(shader.program_c, (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures_[i].id);
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::SetupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(POSITION);
	glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	
	glEnableVertexAttribArray(NORMAL);
	glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(TEX_COORD);
	glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glEnableVertexAttribArray(TANGENT);
	glVertexAttribPointer(TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	glEnableVertexAttribArray(BITANGENT);
	glVertexAttribPointer(BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);
}

void AnimatedMesh::SetupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	//glGenBuffers(1, &BONE_VB);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

	//glBindBuffer(GL_ARRAY_BUFFER, BONE_VB);
	//glBufferData(GL_ARRAY_BUFFER, Bones_.size() * sizeof(VertexBoneData), &Bones_[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(POSITION);
	glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(NORMAL);
	glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(TEX_COORD);
	glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glEnableVertexAttribArray(TANGENT);
	glVertexAttribPointer(TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	glEnableVertexAttribArray(BITANGENT);
	glVertexAttribPointer(BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	//glEnableVertexAttribArray(BONE_ID);
	//glVertexAttribIPointer(BONE_ID, 4, GL_INT, sizeof(VertexBoneData), (void*)0);

	//glEnableVertexAttribArray(BONE_WEIGHT);
	//glVertexAttribPointer(BONE_WEIGHT, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)offsetof(VertexBoneData, Weights));

	glBindVertexArray(0);
}

//This method interpolates the quaternion of rotation of the specified channel according to the animation time
//First we find the key quaternion index, which is up to the required animation time.
//We calculate the ratio between the distance from this key to the required time and the distance 
//from the animation time to the next key. When interpolating this coefficient will be used.
void AnimatedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim) {
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}
	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	if (NextRotationIndex > pNodeAnim->mNumRotationKeys) {
		throw GameException(__LINE__, __func__, string("Error: NextRotationIndex > mNumRotationKeys"));
	}
	float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	if (Factor < 0.0f || Factor > 1.0f) {
		throw GameException(__LINE__, __func__, string("Error: factor"));
	}
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void AnimatedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
	if (NextScalingIndex > pNodeAnim->mNumScalingKeys) {
		throw GameException(__LINE__, __func__, "Error: NextScalingIndex > mNumScalingKeys");
	}
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	if (Factor < 0.0f || Factor > 1.0f) {
		throw GameException(__LINE__, __func__, "Error: Factor");
	}
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void AnimatedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim) {
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	if (NextPositionIndex > pNodeAnim->mNumPositionKeys) {
		throw GameException(__LINE__, __func__, "Error: NextPositionIndex > mNumPositionKeys");
	}
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	if (Factor < 0.0f || Factor > 1.0f) {
		throw GameException(__LINE__, __func__, "Error: Factor");
	}
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

//This function bypasses the sheets of the tree and generates the final transformation for
//each leaf / bone according to the specified animation time.It is limited in the sense that
//we can only use 1 animation sequence.
void AnimatedMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform,
	aiAnimation* pAnimation, aiMatrix4x4& GlobalInverseTransform) {
	string NodeName(pNode->mName.data);
	aiMatrix4x4 NodeTransformation(pNode->mTransformation);
	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		aiMatrix4x4 ScalingM;
		InitScaleTransform(Scaling.x, Scaling.y, Scaling.z, ScalingM);
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		aiMatrix4x4 RotationM = aiMatrix4x4(RotationQ.GetMatrix());
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		aiMatrix4x4 TranslationM;
		InitTranslationTransform(Translation.x, Translation.y, Translation.z, TranslationM);
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (BoneMapping_.find(NodeName) != BoneMapping_.end()) {
		uint BoneIndex = BoneMapping_[NodeName];

		BoneInfo_[BoneIndex].FinalTransformation = GlobalInverseTransform * GlobalTransformation * BoneInfo_[BoneIndex].BoneOffset;
	}
	for (uint i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation, pAnimation, GlobalInverseTransform);
	}
}

//This additional method finds the key rotation just before the animation time.
//If we have N key rotations, then the result can be from 0 to N - 2
uint AnimatedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim) {
	if (pNodeAnim->mNumRotationKeys < 0) {
		throw GameException(__LINE__, __func__, string("Error: mNumRotationKeys < 0"));
	}

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	if (DEBUG_SKINNING) {
		throw GameException(__LINE__, __func__, string("Error: to mush elements"));
	}
	return 0;
}

uint AnimatedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim) {
	if (pNodeAnim->mNumScalingKeys < 0) {
		throw GameException(__LINE__, __func__, "Error: mNumScalingKeys < 0");
	}

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	if (DEBUG_SKINNING) {
		throw GameException(__LINE__, __func__, string("Error: to mush elements"));
	}
	return 0;
}

uint AnimatedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim) {
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	if (DEBUG_SKINNING) {
		throw GameException(__LINE__, __func__, string("Error: to mush elements"));
	}
	return 0;
}

const aiNodeAnim* AnimatedMesh::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName) {
	for (uint i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}
	return NULL;
}

//When called, the current time in seconds and array of matrices are specified,
//which we need to update. We will find the relative time within the animation loop and processing the hierarchy sheets.
//The result is an array of transformations that return to the place of the call
void AnimatedMesh::BoneTransform(float TimeInSeconds, vector<aiMatrix4x4>& Transforms,
	aiAnimation* pAnimation, aiScene* scene, aiMatrix4x4& GlobalInverseTransform) {
	aiMatrix4x4 Identity;
	InitIdentity(Identity);
	float TicksPerSecond = pAnimation->mTicksPerSecond != 0 ? pAnimation->mTicksPerSecond : 25.0f;
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, pAnimation->mDuration);
	ReadNodeHeirarchy(AnimationTime, scene->mRootNode, Identity, pAnimation, GlobalInverseTransform);
	Transforms.resize(NumBones_);
	for (uint i = 0; i < NumBones_; i++) {
		Transforms[i] = BoneInfo_[i].FinalTransformation;
	}
}