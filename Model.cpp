#include "Model.h"

void GameModel::Draw(const GameShader& shader) {
	for (size_t i = 0; i < meshes_.size(); i++) {
		meshes_[i].Draw(shader);
	}
}

void GameModel::LoadModel() {
	const aiScene* scene = importer.ReadFile(path_, \
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | \
		aiProcess_GenNormals | aiProcess_OptimizeMeshes | aiProcess_FindInvalidData | aiProcess_ImproveCacheLocality);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		throw GameException(__LINE__, __FILE__, "Error Assimp, Error:", importer.GetErrorString());
	}
	//GlobalInverseTransform = scene->mRootNode->mTransformation;
	//GlobalInverseTransform.Inverse();

	directory_ = path_.substr(0, path_.find_last_of('\\'));
	ProcessNode(scene->mRootNode, scene);
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
		throw GameException(__LINE__, __FILE__, string("Error load path: ") + path, string("Why: ") + string(stbi_failure_reason()));
	}

	return textureID;
}

void GameModel::PrintModel() {
	if (type_ != 0) {
		print(string("Wrong Class!"));
	}
	string str = string("path: ") + path_ + string("\n");
	str += string("place: ") + vec3_toString(place_) + string("\n");
	str += string("quat: ") + vec3_toString(quat_) + string("\n");
	str += string("scale: ") + vec3_toString(scale_) + string("\n");
	str += string("type: ") + to_string(type_) + string("\n");
	print(str);
}

void Structure::PrintModel() {
	if (type_ != 3) {
		print(string("Wrong Class!"));
	}
	string str = string("path: ") + path_ + string("\n");
	str += string("place: ") + vec3_toString(place_) + string("\n");
	str += string("quat: ") + vec3_toString(quat_) + string("\n");
	str += string("scale: ") + vec3_toString(scale_) + string("\n");
	str += string("type: ") + to_string(type_) + string("\n");
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

//void GameModel::LoadBones(uint MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)
//{
//	for (uint i = 0; i < pMesh->mNumBones; i++) {
//		uint BoneIndex = 0;
//		string BoneName(pMesh->mBones[i]->mName.data);
//
//		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
//			BoneIndex = m_NumBones;
//			m_NumBones++;
//			BoneInfo bi;
//			m_BoneInfo.push_back(bi);
//		}
//		else {
//			BoneIndex = m_BoneMapping[BoneName];
//		}
//
//		m_BoneMapping[BoneName] = BoneIndex;
//		m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
//
//		for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
//			uint VertexID = m_Entries[MeshIndex].BaseVertex +
//				pMesh->mBones[i]->mWeights[j].mVertexId;
//			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
//			Bones[VertexID].AddBoneData(BoneIndex, Weight);
//		}
//	}
//}