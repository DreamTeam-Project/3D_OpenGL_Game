#include "Model.h"

GameModel::GameModel(phys_body* psmodel, GameModel* grmodel, bool draw) : 
	draw_(draw),
	shininess_(grmodel->shininess_),
	path_(grmodel->path_),
	quat_(grmodel->quat_),
	scale_(grmodel->scale_),
	type_(grmodel->type_),
	rigid_body_(psmodel),
	meshes_(grmodel->meshes_)
{	 }

GameModel::GameModel(const GameModel* model, const vec3& place, const vec3& quat, const vec3& scale, bool draw) :
	quat_(quat), scale_(scale), draw_(draw), scene_(model->scene_), meshes_(model->meshes_)
	{   }

GameModel::GameModel(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path,
	const vec3& scale, const double& mass, const vec3& box, float shininess, bool draw) :
	draw_(draw),
	shininess_(shininess),
	path_(path),
	quat_(quat),
	scale_(scale),
	type_(type)
{
	rigid_body_ = new phys_body(real_world_, btVector3(place.x, place.y, place.z), btVector3(box.x, box.y, box.z), btScalar(mass));
}

GameModel::GameModel(phys_world& real_world_, const vec3& place, const vec3& quat, const string& path,
	const vec3& scale, const double& mass, const vec3& box, float shininess, bool draw) :
	draw_(draw),
	shininess_(shininess),
	path_(path),
	quat_(quat),
	scale_(scale) {  }

void GameModel::Move(mat4& model, float deltaTime) {
	model = glm::translate(model, rigid_body_->get_pos());
	quat_ = rigid_body_->get_quat();
	model = glm::rotate(model, quat_.x, vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, quat_.y, vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, quat_.z, vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, scale_);
}

void Structure::Move(mat4& model, float deltaTime) {
	model = model_;
}

Structure::Structure(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path,
	const vec3& scale, const double& mass, const vec3& box, float shininess, bool draw) :
	GameModel(real_world_, type, place, quat, path, scale, mass, box, shininess, draw)
{
	model_ = glm::translate(model_, place);
	model_ = glm::rotate(model_, glm::radians(quat.x), vec3(1.0f, 0.0f, 0.0f));
	model_ = glm::rotate(model_, glm::radians(quat.y), vec3(0.0f, 1.0f, 0.0f));
	model_ = glm::rotate(model_, glm::radians(quat.z), vec3(0.0f, 0.0f, 1.0f));
	model_ = glm::scale(model_, scale);
}

void GameModel::Draw(const GameShader& shader) {
	for (uint i = 0; i < meshes_.size(); i++) {
		meshes_[i]->Draw(shader);
	}
}

void GameModel::ClearLoaded() {
	textures_loaded_.clear();
}

void GameModel::CopyModel(const GameModel* model) {
	meshes_ = model->meshes_;
}

void GameModel::LoadModel() {
	Assimp::Importer importer_;
	scene_ = importer_.ReadFile(path_, ASSIMP_FLAGS);
	if (!scene_ || scene_->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene_->mRootNode) {
		throw GameException(__LINE__, __func__, "Error Assimp, Error:", importer_.GetErrorString());
	}
	directory_ = path_.substr(0, path_.find_last_of('\\'));
	ProcessNode(scene_->mRootNode, scene_);
}

void GameModel::SetShaderParameters(const GameShader& shader, float deltaTime) {
	mat4 model;
	Move(model, deltaTime);
	shader.setMat4("model", model);
	shader.setFloat("material.shininess", shininess_);
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

void GameModel::ProcessNode(aiNode *node, const aiScene *scene) {
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

	return CreateMesh(vertices, indices, textures);;
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