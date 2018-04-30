#ifndef MESH_H
#define MESH_H

#define GLEW_STATIC

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "System.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using std::vector;
using std::stringstream;

struct Texture {
	unsigned int id;
	string type;
	string path;
};

class Mesh {
public:
	vector<Vertex> vertices_c;
	vector<unsigned int> indices_c;
	vector<Texture> textures_c;

	Mesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures)
		: vertices_c(vertices), indices_c(indices), textures_c(textures) {
#if DEBUG_MESH
		print(string("sizeof indices: " + std::to_string(indices_c.size())));
#endif
		SetupMesh();
	}

	void Draw(const GameShader& shader); 	// render the mesh

private:
	unsigned int VBO, EBO, VAO;
	void SetupMesh();	// initializes all the buffer objects/arrays
};

#endif