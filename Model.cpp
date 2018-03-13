#include "Model.h"

void Model_t::Draw(Shader_t shader) {
	for (size_t i = 0; i < meshes_c.size(); i++) {
		meshes_c[i].Draw(shader);
	}
}

void Model_t::LoadModels(Map* objects) {
	Assimp::Importer importer;
	int i = 0;
	for (auto it : objects->object) {
		const aiScene* scene = importer.ReadFile(it, \
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace |\
			aiProcess_GenNormals | aiProcess_OptimizeMeshes | aiProcess_FindInvalidData);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			throw Exception_t(__LINE__, __FILE__, "ERROR::ASSIMP::", importer.GetErrorString());
		}
		directory_c = it.substr(0, it.find_last_of('/'));
		ProcessNode(scene->mRootNode, scene, objects->place[i]);
		i++;
	}
}

void Model_t::ProcessNode(aiNode *node, const aiScene *scene, vec3 place) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes_c.push_back(ProcessMesh(mesh, scene, place));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene, place);
	}
}

Mesh_t Model_t::ProcessMesh(aiMesh *mesh, const aiScene *scene, vec3 place) {
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vec3 vector;
		vector.x = mesh->mVertices[i].x + place.x;
		vector.y = mesh->mVertices[i].y + place.y;
		vector.z = mesh->mVertices[i].z + place.z;
		vertex.Position = vector;
		vector.x = mesh->mNormals[i].x + place.x;
		vector.y = mesh->mNormals[i].y + place.y;
		vector.z = mesh->mNormals[i].z + place.z;
		vertex.Normal = vector;
		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x + place.x;
			vec.y = mesh->mTextureCoords[0][i].y + place.y;
			vertex.TexCoords = vec;
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		if (mesh->mTangents != NULL) {
			vector.x = mesh->mTangents[i].x + place.x;
			vector.y = mesh->mTangents[i].y + place.y;
			vector.z = mesh->mTangents[i].z + place.z;
			vertex.Tangent = vector;
		}
		else {
			vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		if(mesh->mBitangents != NULL) {
			vector.x = mesh->mBitangents[i].x + place.x;
			vector.y = mesh->mBitangents[i].y + place.y;
			vector.z = mesh->mBitangents[i].z + place.z;
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

	return Mesh_t(vertices, indices, textures);
}

vector<Texture> Model_t::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName) {
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded_c.size(); j++) {
			if (std::strcmp(textures_loaded_c[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded_c[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory_c);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded_c.push_back(texture);
		}
	}
	return textures;
}

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma) {
	string filename = string(path);
	filename = directory + '/' + filename;

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
		throw Exception_t(__LINE__, __FILE__, "ERROR::LOAD::FAILED::PATH::", path);
	}

	return textureID;
}