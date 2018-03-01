#define GLEW_STATIC
#include "Shader.h"

Shader_t::Shader_t(const GLchar* vertexPath, const GLchar* fragmentPath) {
	string vertexCode;
	string fragmentCode;
	ifstream vShaderFile;
	ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::badbit);
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
	GLchar infoLog[512];
	//Vertex Program
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		throw Exception_t(__LINE__, __FILE__, "SHADER::VERTEX::COMPILATION_FAILED", infoLog);
	}
	//Fragment Program
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		throw Exception_t(__LINE__, __FILE__, "SHADER::FRAGMENT::COMPILATION_FAILED", infoLog);
	}
	// Shader Program
	this->Program = glCreateProgram();
	glAttachShader(this->Program, vertex);
	glAttachShader(this->Program, fragment);
	glLinkProgram(this->Program);
	glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
		throw Exception_t(__LINE__, __FILE__, "SHADER::PROGRAM::LINKING_FAILED", infoLog);
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader_t::Use() { 
	glUseProgram(this->Program); 
}