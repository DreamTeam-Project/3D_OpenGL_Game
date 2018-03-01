#define GLEW_STATIC

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <gl/glew.h>
#include "Types.h"

using std::string;
using std::ifstream;

class Shader_t {
public:
	GLuint Program;
	Shader_t(const GLchar* vertexPath, const GLchar* fragmentPath);
	void Use();
};

#endif