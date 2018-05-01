#include "Model.h"

void GameModel::Draw(const GameShader& shader) {
	for (size_t i = 0; i < meshes_.size(); i++) {
		meshes_[i].Draw(shader);
	}
}

void GameModel::LoadModel() {
	scene_ = importer_.ReadFile(path_, \
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | \
		aiProcess_GenNormals | aiProcess_OptimizeMeshes | aiProcess_FindInvalidData | aiProcess_ImproveCacheLocality);
	if (!scene_ || scene_->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene_->mRootNode) {
		throw GameException(__LINE__, __func__, "Error Assimp, Error:", importer_.GetErrorString());
	}
	GlobalInverseTransform_ = scene_->mRootNode->mTransformation;
	GlobalInverseTransform_.Inverse();

	directory_ = path_.substr(0, path_.find_last_of('\\'));
	ProcessNode(scene_->mRootNode, scene_);
}

void GameModel::ProcessNode(aiNode *node, const aiScene *scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes_.push_back(ProcessMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh GameModel::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		if (mesh->mTangents != NULL) {
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
		}
		else {
			vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		if (mesh->mBitangents != NULL) {
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else {
			vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"); 	// 1. diffuse maps
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular"); 	// 2. specular maps
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal"); 	// 3. normal maps
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");	// 4. height maps
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	return Mesh(vertices, indices, textures);
}

vector<Texture> GameModel::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, const string& typeName) {
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded_.size(); j++) {
			if (std::strcmp(textures_loaded_[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded_[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory_);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded_.push_back(texture);
		}
	}
	return textures;
}

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma) {
	string filename = string(path);
	filename = directory + '\\' + filename;
#if DEBUG_MODEL
	print(string("directory: ") + directory);
	print(string("filename: ") + filename);
#endif
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1) {
			format = GL_RED;
		}
		else if (nrComponents == 3) {
			format = GL_RGB;
		}
		else if (nrComponents == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		stbi_image_free(data);
		throw GameException(__LINE__, __func__, string("Error load path: ") + path, string("Why: ") + string(stbi_failure_reason()));
	}

	return textureID;
}

void GameModel::PrintModel() {
	if (type_ != GAMEMODEL) {
		print(string("Wrong Class!"));
	}
	string str = string("type: ") + to_string(type_) + string(" - GameModel") + string("\n");
	str += string("path: ") + path_ + string("\n");
	str += string("place: ") + vec3_toString(place_) + string("\n");
	str += string("quat: ") + vec3_toString(quat_) + string("\n");
	str += string("scale: ") + vec3_toString(scale_) + string("\n");
	print(str);
}

void Structure::PrintModel() {
	if (type_ != STRUCTURE) {
		print(string("Wrong Class!"));
	}
	string str = string("type: ") + to_string(type_) + string(" - Structure") + string("\n");
	str += string("path: ") + path_ + string("\n");
	str += string("place: ") + vec3_toString(place_) + string("\n");
	str += string("quat: ") + vec3_toString(quat_) + string("\n");
	str += string("scale: ") + vec3_toString(scale_) + string("\n");
	print(str);
}

void Animation::PrintModel() {
	if (type_ != ANIMATION) {
		print(string("Wrong Class!"));
	}
	string str = string("type: ") + to_string(type_) + string(" - Animation") + string("\n");
	str += string("path: ") + path_ + string("\n");
	str += string("place: ") + vec3_toString(place_) + string("\n");
	str += string("quat: ") + vec3_toString(quat_) + string("\n");
	str += string("scale: ") + vec3_toString(scale_) + string("\n");
	print(str);
}

void GameModel::Move(mat4& model) {
	model = glm::translate(model, place_);
	model = glm::rotate(model, glm::radians(quat_.x), vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, glm::radians(quat_.y), vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(quat_.z), vec3(0.0, 0.0, 1.0));
	model = glm::scale(model, scale_);
}

void Structure::Move(mat4& model) {
	model = glm::translate(model, place_);
	model = glm::rotate(model, glm::radians(quat_.x), vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, glm::radians(quat_.y), vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(quat_.z), vec3(0.0, 0.0, 1.0));
	model = glm::scale(model, scale_);
}

void Animation::Move(mat4& model) {
	model = glm::translate(model, place_);
	model = glm::rotate(model, glm::radians(quat_.x), vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, glm::radians(quat_.y), vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(quat_.z), vec3(0.0, 0.0, 1.0));
	model = glm::scale(model, scale_);
}

//%%%%%%%%%
//Skinning
//%%%%%%%%%

//When called, the current time in seconds and array of matrices are specified,
//which we need to update. We will find the relative time within the animation loop and processing the hierarchy sheets.
//The result is an array of transformations that return to the place of the call
void GameModel::BoneTransform(float TimeInSeconds, vector<aiMatrix4x4>& Transforms) {
	aiMatrix4x4 Identity;
	InitIdentity(Identity);
	float TicksPerSecond = scene_->mAnimations[0]->mTicksPerSecond != 0 ? scene_->mAnimations[0]->mTicksPerSecond : 25.0f;
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, scene_->mAnimations[0]->mDuration);
	ReadNodeHeirarchy(AnimationTime, scene_->mRootNode, Identity);
	Transforms.resize(NumBones_);
	for (uint i = 0; i < NumBones_; i++) {
		Transforms[i] = BoneInfo_[i].FinalTransformation;
	}
}

//The function below loads the bone information for one aiMesh object.
//In addition to filling the VertexBoneData structure, this function also updates the links between the name of the bone and
//ID number (the index is determined at startup) and writes the displacement matrix to a vector depending on the bone id.
void GameModel::LoadBones(uint MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones) {
	for (uint i = 0; i < pMesh->mNumBones; i++) {
		uint BoneIndex = 0;
		string BoneName(pMesh->mBones[i]->mName.data);

		if (BoneMapping_.find(BoneName) == BoneMapping_.end()) {
			BoneIndex = NumBones_;
			NumBones_++;
			BoneInfo bi;
			BoneInfo_.push_back(bi);
		}
		else {
			BoneIndex = BoneMapping_[BoneName];
		}

		BoneMapping_[BoneName] = BoneIndex;
		BoneInfo_[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;

		for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			uint VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}

//This helper function finds free slots in the VertexBoneData structure and places inside
//the id and bone weight.
void GameModel::VertexBoneData::AddBoneData(uint BoneID, float Weight)
{
	for (uint i = 0; i < ArraySizeInElements(IDs); i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}
	throw GameException(__LINE__, __func__, string("Error: to mush bones"));
}

//This method interpolates the quaternion of rotation of the specified channel according to the animation time
//First we find the key quaternion index, which is up to the required animation time.
//We calculate the ratio between the distance from this key to the required time and the distance 
//from the animation time to the next key. When interpolating this coefficient will be used.
void GameModel::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim) {
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
	if (Factor < 0.0f && Factor > 1.0f) {
		throw GameException(__LINE__, __func__, string("Error: factor"));
	}
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void GameModel::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
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
	if (Factor < 0.0f && Factor > 1.0f) {
		throw GameException(__LINE__, __func__, "Error: Factor");
	}
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void GameModel::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim) {
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
	if(Factor < 0.0f && Factor > 1.0f) {
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
void GameModel::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform) {
	string NodeName(pNode->mName.data);
	const aiAnimation* pAnimation = scene_->mAnimations[0];
	aiMatrix4x4 NodeTransformation(pNode->mTransformation);
	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Интерполируем масштабирование и генерируем матрицу преобразования масштаба
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		aiMatrix4x4 ScalingM;
		InitScaleTransform(Scaling.x, Scaling.y, Scaling.z, ScalingM);
		// Интерполируем вращение и генерируем матрицу вращения
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		aiMatrix4x4 RotationM = aiMatrix4x4(RotationQ.GetMatrix());
		//  Интерполируем смещение и генерируем матрицу смещения
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		aiMatrix4x4 TranslationM;
		InitTranslationTransform(Translation.x, Translation.y, Translation.z, TranslationM);
		// Объединяем преобразования
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (BoneMapping_.find(NodeName) != BoneMapping_.end()) {
		uint BoneIndex = BoneMapping_[NodeName];

		BoneInfo_[BoneIndex].FinalTransformation = GlobalInverseTransform_ *
			GlobalTransformation *
			BoneInfo_[BoneIndex].BoneOffset;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}

//This additional method finds the key rotation just before the animation time.
//Если мы имеем N ключевых вращений, то результат может быть от 0 до N - 2
uint GameModel::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim) {
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

uint GameModel::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim) {
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

uint GameModel::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim) {
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

const aiNodeAnim* GameModel::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName) {
	for (uint i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}
	return NULL;
}