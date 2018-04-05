#ifndef Types_H
#define Types_H

#include <string>
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

#define SUCCESS 42

using std::vector;
using glm::vec3;
using std::string;
using std::exception;
using std::ios_base;
using std::ofstream;
using std::ifstream;
using std::to_string;

class Exception_t {
public:
	Exception_t(size_t LINE, string FILE, string INF = "default") : LINE_(LINE), FILE_(FILE), INF_(INF) { }
	Exception_t(size_t LINE, string FILE, string INF1, string INF2) : LINE_(LINE), FILE_(FILE), INF_(INF1 + INF2) { }
	string what() {
		string tmp = string("Line:") + std::to_string(LINE_) + string("\nFile:") + FILE_ + string("\nInfo:") + INF_;
		return tmp;
	}
private:
	size_t LINE_;
	string FILE_;
	string INF_;
};

bool IsItNumber(const string& word);
int doNothing();
void print(const char* what);
void print(const string& what);
void getStringFromFile(ifstream& fin, string& ret);
void getStringFromFile(ifstream& fin, int& ret);
void getStringFromFile(ifstream& fin, glm::vec3& ret);
string write(const vec3& a);

extern GLFWwindow* game_window;
const string LoadFile = "Load.file";

struct Level {
	string name_;
	string pathLoader_;
	Level(string name = "default", string path = "default") : name_(name), pathLoader_(path) { }
};

struct Levels {
	vector<Level> levels_;
	void push_back(Level one) {
		if (one.name_ != "null" && one.pathLoader_ != "null") {
			levels_.push_back(one);
		}
		else {
			throw Exception_t(__LINE__, __FILE__, "error push back level");
		}
	}
};

struct Object {
	int type_;
	string path_;
	vec3 quat_;
	vec3 place_;
	Object(int type = 0, string path = "default", vec3 quat = { 0.0f, 0.0f, 0.0f }, vec3 place = { 0, 0, 0 }) :
		type_(type), path_(path), quat_(quat), place_(place) { }
};

struct Objects {
	vector<Object> LoadedInfo;
};

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	void addvec(vec3 a);
};

#endif