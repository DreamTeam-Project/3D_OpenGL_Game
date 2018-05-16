#include "Mesh.h"

Mesh* CreateMesh(vector<Vertex>& vertices, vector<uint>& indices, vector<GameTexture>& textures) {
	Mesh* ret = new Mesh(vertices, indices, textures);
	ret->SetupMesh();
	return ret;
}

void Mesh::Draw(const GameShader& shader) {
	uint diffuseNr = 1;
	uint specularNr = 1;
	for (uint i = 0; i < textures_.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		string number;
		string name = textures_[i].type;
		if (name == "diffuse") {
			number = to_string(diffuseNr++);
		}
		else if (name == "specular") {
			number = to_string(specularNr++);
		}
		glUniform1i(glGetUniformLocation(shader.program_c, (string("material.") + name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures_[i].id);
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::SetupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(uint), &indices_[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(POSITION);
	glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	
	glEnableVertexAttribArray(NORMAL);
	glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(TEX_COORD);
	glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}