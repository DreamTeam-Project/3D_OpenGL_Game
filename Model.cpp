#include "Model.h"

void GameModel::Draw(const GameShader& shader) {
	for (uint i = 0; i < meshes_.size(); i++) {
		meshes_[i]->Draw(shader);
	}
}

void GameModel::ClearLoaded() {
	textures_loaded_.clear();
}

void GameModel::CopyModel(const GameModel* model) {
	Entries_ = model->Entries_;
	meshes_ = model->meshes_;
	GlobalInverseTransform_ = model->GlobalInverseTransform_;
	scene_ = model->scene_;
}

GameModel::GameModel(const GameModel* model, vec3 place, vec3 quat, vec3 scale, bool draw) :
	quat_(quat), scale_(scale), draw_(draw), scene_(model->scene_),
	Entries_(model->Entries_), meshes_(model->meshes_), GlobalInverseTransform_(model->GlobalInverseTransform_)
	{   }

void GameModel::LoadModel() {
	Assimp::Importer importer_;
	scene_ = importer_.ReadFile(path_, ASSIMP_FLAGS);
	if (!scene_ || scene_->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene_->mRootNode) {
		throw GameException(__LINE__, __func__, "Error Assimp, Error:", importer_.GetErrorString());
	}
	if (scene_->mAnimations != nullptr) {
		GlobalInverseTransform_ = scene_->mRootNode->mTransformation;
		GlobalInverseTransform_.Inverse();
	}
	directory_ = path_.substr(0, path_.find_last_of('\\'));
	ProcessNode(scene_->mRootNode, scene_);
}

void GameModel::ProcessNode(aiNode *node, const aiScene *scene) {
	uint NumVertices = 0;
	uint NumIndices = 0;
	Entries_.resize(scene->mNumMeshes);
	for (uint i = 0; i < Entries_.size(); i++) {
		Entries_[i].MaterialIndex = scene->mMeshes[i]->mMaterialIndex;
		Entries_[i].NumIndices = scene->mMeshes[i]->mNumFaces * 3;
		Entries_[i].BaseVertex = NumVertices;
		Entries_[i].BaseIndex = NumIndices;

		NumVertices += scene->mMeshes[i]->mNumVertices;
		NumIndices += Entries_[i].NumIndices;
	}

	for (uint i = 0; i < scene->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[i];
		meshes_.push_back(ProcessMesh(mesh, scene, i));
	}
}

Mesh* GameModel::ProcessMesh(aiMesh *mesh, const aiScene *scene, uint MeshIndex) {
	Mesh* ret = nullptr;
	vector<Vertex> vertices;
	vector<uint> indices;
	vector<GameTexture> textures;

	indices.reserve(mesh->mNumFaces * 3);
	vertices.reserve(mesh->mNumVertices);

	for (uint i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (mesh->HasTextureCoords(0)) {
			vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}

	for (uint i = 0; i < mesh->mNumFaces; i++) {
		indices.push_back(mesh->mFaces[i].mIndices[0]);
		indices.push_back(mesh->mFaces[i].mIndices[1]);
		indices.push_back(mesh->mFaces[i].mIndices[2]);
	}
	
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	vector<GameTexture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	vector<GameTexture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	if (scene_->mAnimations != nullptr) {
		vector<VertexBoneData> Bones;
		vector<BoneInfo> BonesInfo;
		map<string, uint> BoneMapping;
		uint NumBones = 0;
		Bones.resize(mesh->mNumVertices);
		LoadBones(MeshIndex, mesh, Bones, BonesInfo, BoneMapping, NumBones);

		ret = CreateAnimatedMesh(vertices, indices, textures, BonesInfo, Bones, NumBones, BoneMapping);
	}
	else {
		ret = CreateMesh(vertices, indices, textures);
	}

	return ret;
}

vector<GameTexture> GameModel::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, const string& typeName) {
	vector<GameTexture> textures;
	for (uint i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (uint j = 0; j < textures_loaded_.size(); j++) {
			if (std::strcmp(textures_loaded_[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded_[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			GameTexture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory_);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded_.push_back(texture);
		}
	}
	return textures;
}

uint TextureFromFile(const char *path, const string &directory) {
	string filename = string(path);
	filename = directory + '\\' + filename;
#if DEBUG_MODEL
	print(string("directory: ") + directory);
	print(string("filename: ") + filename);
#endif
	uint textureID;
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
	str += string("place: ") + vec3_toString(rigid_body_->get_pos()) + string("\n");
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
	str += string("place: ") + vec3_toString(rigid_body_->get_pos()) + string("\n");
	str += string("quat: ") + vec3_toString(quat_) + string("\n");
	str += string("scale: ") + vec3_toString(scale_) + string("\n");
	print(str);
}

void AnimatedModel::PrintModel() {
	if (type_ != ANIMATION) {
		print(string("Wrong Class!"));
	}
	string str = string("type: ") + to_string(type_) + string(" - Animation") + string("\n");
	str += string("path: ") + path_ + string("\n");
	str += string("place: ") + vec3_toString(rigid_body_->get_pos()) + string("\n");
	str += string("quat: ") + vec3_toString(quat_) + string("\n");
	str += string("scale: ") + vec3_toString(scale_) + string("\n");
	print(str);
}

void GameModel::Move(mat4& model) {
	model = glm::translate(model, rigid_body_->get_pos());
	model = glm::rotate(model, glm::radians(quat_.x), vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(quat_.y), vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(quat_.z), vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, scale_);
}

void GameModel::SetShaderParameters(const GameShader& shader) {
	mat4 model;
	Move(model);
	shader.setMat4("model", model);
	shader.setFloat("material.shininess", shininess_);
}

//The function below loads the bone information for one aiMesh object.
//In addition to filling the VertexBoneData structure, this function also updates the links between the name of the bone and
//ID number (the index is determined at startup) and writes the displacement matrix to a vector depending on the bone id.
void GameModel::LoadBones(uint MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones,
	vector<BoneInfo>& BonesInfo, map<string, uint>& BoneMapping, uint& NumBones) {
	for (uint i = 0; i < pMesh->mNumBones; i++) {
		uint BoneIndex = 0;
		string BoneName(pMesh->mBones[i]->mName.data);
#if DEBUG_SKINNING
		print(string("bone: ") + BoneName);
#endif
		if (BoneMapping.find(BoneName) == BoneMapping.end()) {
			BoneIndex = NumBones;
			NumBones++;
			BoneInfo bi;
			BonesInfo.push_back(bi);
			BonesInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
			BoneMapping[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = BoneMapping[BoneName];
		}

		for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			uint VertexID = Entries_[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}