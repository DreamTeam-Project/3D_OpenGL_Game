#include "Mesh.h"

void Mesh::Draw(const GameShader& shader) {
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < textures_c.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		string number;
		string name = textures_c[i].type;
		if      (name == "texture_diffuse") {
			number = std::to_string(diffuseNr++);
		}
		else if (name == "texture_specular") {
			number = std::to_string(specularNr++);
		}
		else if (name == "texture_normal") {
			number = std::to_string(normalNr++);
		}
		else if (name == "texture_height") {
			number = std::to_string(heightNr++);
		}
		glUniform1i(glGetUniformLocation(shader.program_c, (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures_c[i].id);
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices_c.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::SetupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices_c.size() * sizeof(Vertex), &vertices_c[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_c.size() * sizeof(unsigned int), &indices_c[0], GL_DYNAMIC_DRAW);
	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glBindVertexArray(0);
}