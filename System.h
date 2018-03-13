#ifndef Types_H
#define Types_H

#include <string>
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

using std::vector;
using std::string;
using std::exception;
using std::ios_base;
using std::ofstream;

struct Map {
	vector<string> object;
	vector<glm::vec3> place;
};

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

extern GLFWwindow* game_window;

void PrintException(string& exc);

class Exception_t {
public:
	Exception_t(size_t LINE, string FILE, string INF = "default") : LINE_(LINE), FILE_(FILE), INF_(INF) { }
	Exception_t(size_t LINE, string FILE, string INF1, string INF2) : LINE_(LINE), FILE_(FILE), INF_(INF1 + INF2) { }
	string what() { 
		string tmp = std::to_string(LINE_) + "::" + FILE_ + "\n" + INF_;
		return tmp;
	}
private:
	size_t LINE_;
	string FILE_;
	string INF_;
};

#endif