#ifndef MESH_H
#define MESH_H

#define GLEW_STATIC

enum Location {
	POSITION = 0, 
	NORMAL, 
	TEX_COORD, 
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

struct GameTexture {
	uint id;
	string type;
	string path;
};
struct Vertex {
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
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

protected:
	uint VBO, EBO, VAO;
};

Mesh* CreateMesh(vector<Vertex>& vertices, vector<uint>& indices, vector<GameTexture>& textures);

#endif