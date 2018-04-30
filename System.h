#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

#define SUCCESS 42
#define DEBUG_MODEL 0
#define DEBUG_MESH 0
#define DEBUG_MANAGER 0

//#define NUM_BONES_PER_VEREX 4
//typedef unsigned int uint;

using std::vector;
using glm::vec3;
using std::string;
using std::exception;
using std::ios_base;
using std::ofstream;
using std::ifstream;
using std::to_string;


class GameException : public exception
{
public:
	GameException() = delete;
	GameException(string &&whatStr) noexcept : whatStr_(std::move(whatStr)) { }
	GameException(const string &whatStr) noexcept : whatStr_(whatStr) { }
	GameException(size_t LINE, const string& FILE, const string& INF1 = "", const string& INF2 = "") noexcept {
		whatStr_ = string("Line: ") + std::to_string(LINE) + string("\nFILE: ") + FILE + string("\nWhy: ") + INF1 + string(" ") + INF2;
	};

	const char* what() const noexcept override {
		return whatStr_.c_str();
	}
	~GameException() noexcept = default;
private:
	string whatStr_;
};

bool IsItNumber(const string& word);
int doNothing();
void print(const char* what);
void print(const string& what);
void getStringFromFile(ifstream& fin, string& ret);
void getStringFromFile(ifstream& fin, int& ret);
void getStringFromFile(ifstream& fin, vec3& ret);
string vec3_toString(const vec3& a);

extern GLFWwindow* game_window;
const string LoadFile = "Load.file";

//struct VertexBoneData {
//	uint IDs[NUM_BONES_PER_VEREX];
//	float Weights[NUM_BONES_PER_VEREX];
//};

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	void addvec(vec3 a);
};

#endif