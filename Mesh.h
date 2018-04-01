#define GLEW_STATIC

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

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

class Mesh_t {
public:
	vector<Vertex> vertices_c;
	vector<unsigned int> indices_c;
	vector<Texture> textures_c;

	Mesh_t(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures)
		: vertices_c(vertices), indices_c(indices), textures_c(textures) {
		SetupMesh();
	}
	void Draw(const Shader_t& shader); 	// render the mesh

private:
	unsigned int VBO, EBO, VAO;
	void SetupMesh();	// initializes all the buffer objects/arrays
};