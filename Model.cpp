#include "Model.h"

void GameModel::Draw(const GameShader& shader) {
	for (size_t i = 0; i < meshes_.size(); i++) {
		meshes_[i]->Draw(shader);
	}
}

void GameModel::LoadModel() {
	scene_ = importer_.ReadFile(path_, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
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

	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[i];
		meshes_.push_back(ProcessMesh(mesh, scene, i));
	}
}

Mesh* GameModel::ProcessMesh(aiMesh *mesh, const aiScene *scene, uint MeshIndex) {
	Mesh* ret = nullptr;
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<GameTexture> textures;

	indices.reserve(mesh->mNumFaces * 3);
	vertices.reserve(mesh->mNumVertices);

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
		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	vector<GameTexture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"); 	//diffuse maps
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	vector<GameTexture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular"); 	//specular maps
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	vector<GameTexture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal"); 	//normal maps
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	vector<GameTexture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");	//height maps
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

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
	rigid_body_->get_status();
	model = glm::rotate(model, glm::radians(quat_.x), vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, glm::radians(quat_.y), vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(quat_.z), vec3(0.0, 0.0, 1.0));
	model = glm::scale(model, scale_);
}

void Structure::Move(mat4& model) {
	model = glm::translate(model, rigid_body_->get_pos());
	rigid_body_->get_status();
	model = glm::rotate(model, glm::radians(quat_.x), vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, glm::radians(quat_.y), vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(quat_.z), vec3(0.0, 0.0, 1.0));
	model = glm::scale(model, scale_);
}

void AnimatedModel::Move(mat4& model) {
	model = glm::translate(model, rigid_body_->get_pos());
	rigid_body_->get_status();
	model = glm::rotate(model, glm::radians(quat_.x), vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, glm::radians(quat_.y), vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(quat_.z), vec3(0.0, 0.0, 1.0));
	model = glm::scale(model, scale_);
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