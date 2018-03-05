#include "Shader.h"

Shader_t::Shader_t(const char* vertexPath, const char* fragmentPath) {
	string vertexCode;
	string fragmentCode;
	ifstream vShaderFile;
	ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		throw Exception_t(__LINE__, __FILE__, "FILE_NOT_SUCCESFULLY_READ", e.what());
	}
	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar * fShaderCode = fragmentCode.c_str();
	GLuint vertex, fragment;
	GLint success;
	//Vertex Program
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar infoLog[INFOLOG_SIZE];
		glGetShaderInfoLog(vertex, INFOLOG_SIZE, NULL, infoLog);
		throw Exception_t(__LINE__, __FILE__, "SHADER::VERTEX::COMPILATION_FAILED", infoLog);
	}
	//Fragment Program
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar infoLog[INFOLOG_SIZE];
		glGetShaderInfoLog(fragment, INFOLOG_SIZE, NULL, infoLog);
		throw Exception_t(__LINE__, __FILE__, "SHADER::FRAGMENT::COMPILATION_FAILED", infoLog);
	}
	// Shader Program
	this->program_c = glCreateProgram();
	glAttachShader(this->program_c, vertex);
	glAttachShader(this->program_c, fragment);
	glLinkProgram(this->program_c);
	glGetProgramiv(this->program_c, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar infoLog[INFOLOG_SIZE];
		glGetProgramInfoLog(this->program_c, INFOLOG_SIZE, NULL, infoLog);
		throw Exception_t(__LINE__, __FILE__, "SHADER::PROGRAM::LINKING_FAILED", infoLog);
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader_t::Shader_t(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		gShaderFile.open(geometryPath);
		std::stringstream vShaderStream, fShaderStream, gShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		gShaderStream << gShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		gShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		geometryCode = gShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		throw Exception_t(__LINE__, __FILE__, "FILE_NOT_SUCCESFULLY_READ", e.what());
	}
	const char* vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();
	const char * gShaderCode = geometryCode.c_str();
	unsigned int vertex, fragment, geometry;
	int success;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar infoLog[INFOLOG_SIZE];
		glGetShaderInfoLog(vertex, INFOLOG_SIZE, NULL, infoLog);
		throw Exception_t(__LINE__, __FILE__, "SHADER::VERTEX::COMPILATION_FAILED", infoLog);
	}
	// fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	if (!success) {
		GLchar infoLog[INFOLOG_SIZE];
		glGetShaderInfoLog(fragment, INFOLOG_SIZE, NULL, infoLog);
		throw Exception_t(__LINE__, __FILE__, "SHADER::FRAGMENT::COMPILATION_FAILED", infoLog);
	}
	// geometry shader
	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &gShaderCode, NULL);
	glCompileShader(geometry);
	if (!success) {
		GLchar infoLog[INFOLOG_SIZE];
		glGetShaderInfoLog(geometry, INFOLOG_SIZE, NULL, infoLog);
		throw Exception_t(__LINE__, __FILE__, "SHADER::GEOMETRY::COMPILATION_FAILED", infoLog);
	}
	// shader Program
	program_c = glCreateProgram();
	glAttachShader(program_c, vertex);
	glAttachShader(program_c, fragment);
	glAttachShader(program_c, geometry);
	glLinkProgram(program_c);
	glGetProgramiv(this->program_c, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar infoLog[INFOLOG_SIZE];
		glGetProgramInfoLog(this->program_c, INFOLOG_SIZE, NULL, infoLog);
		throw Exception_t(__LINE__, __FILE__, "SHADER::PROGRAM::LINKING_FAILED", infoLog);
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);

}

void Shader_t::Use() { 
	glUseProgram(this->program_c); 
}

void Shader_t::setBool(const std::string &name, bool value) const {
	glUniform1i(glGetUniformLocation(program_c, name.c_str()), (int)value);
}

void Shader_t::setInt(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(program_c, name.c_str()), value);
}

void Shader_t::setfloat(const std::string &name, float value) const {
	glUniform1f(glGetUniformLocation(program_c, name.c_str()), value);
}

void Shader_t::setVec2(const std::string &name, const glm::vec2 &value) const {
	glUniform2fv(glGetUniformLocation(program_c, name.c_str()), 1, &value[0]);
}

void Shader_t::setVec2(const std::string &name, float x, float y) const {
	glUniform2f(glGetUniformLocation(program_c, name.c_str()), x, y);
}

void Shader_t::setVec3(const std::string &name, const glm::vec3 &value) const {
	glUniform3fv(glGetUniformLocation(program_c, name.c_str()), 1, &value[0]);
}

void Shader_t::setVec3(const std::string &name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(program_c, name.c_str()), x, y, z);
}

void Shader_t::setVec4(const std::string &name, const glm::vec4 &value) const {
	glUniform4fv(glGetUniformLocation(program_c, name.c_str()), 1, &value[0]);
}

void Shader_t::setVec4(const std::string &name, float x, float y, float z, float w) {
	glUniform4f(glGetUniformLocation(program_c, name.c_str()), x, y, z, w);
}

void Shader_t::setMat2(const std::string &name, const glm::mat2 &mat) const {
	glUniformMatrix2fv(glGetUniformLocation(program_c, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader_t::setMat3(const std::string &name, const glm::mat3 &mat) const {
	glUniformMatrix3fv(glGetUniformLocation(program_c, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader_t::setMat4(const std::string &name, const glm::mat4 &mat) const {
	glUniformMatrix4fv(glGetUniformLocation(program_c, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}