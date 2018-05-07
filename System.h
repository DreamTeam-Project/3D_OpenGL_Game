#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>
#include <assimp\matrix4x4.h>

#if _DEBUG
#define DEBUG_GAME		1
#define DEBUG_MODEL		0
#define DEBUG_MESH		0
#define DEBUG_MANAGER	0
#define DEBUG_SKINNING	0
#define DEBUG_PHYSICS	1
#endif

#define SUCCESS 42
#define NUM_BONES_PER_VEREX 4

typedef unsigned int uint;

using std::vector;
using glm::vec3;
using std::string;
using std::exception;
using std::ios_base;
using std::ofstream;
using std::ifstream;
using std::to_string;

enum GameEnumClass {
	GAMEMODEL,	STRUCTURE,	ANIMATION
};

class GameException : public exception
{
public:
	GameException() = delete;
	GameException(string &&whatStr) noexcept : whatStr_(std::move(whatStr)) { }
	GameException(const string &whatStr) noexcept : whatStr_(whatStr) { }
	GameException(size_t LINE, const string& func, const string& INF1 = "", const string& INF2 = "") noexcept {
		whatStr_ = string("Line: ") + std::to_string(LINE) + string("\nFunc: ") + func + string("\nWhy: ") + INF1 + string(" ") + INF2;
	};

	const char* what() const noexcept override {
		return whatStr_.c_str();
	}
	~GameException() noexcept = default;
private:
	string whatStr_;
};

string vec3_toString(const vec3& a);
bool IsItNumber(const string& word);
void SetZero(aiMatrix4x4* matrix);
void print(const char* what);
void print(const string& what);
void getStringFromFile(ifstream& fin, string& ret);
void getStringFromFile(ifstream& fin, int& ret);
void getStringFromFile(ifstream& fin, double& ret);
void getStringFromFile(ifstream& fin, vec3& ret);
void InitIdentity(aiMatrix4x4& matrix);
void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ, aiMatrix4x4& matrix);
void InitTranslationTransform(float x, float y, float z, aiMatrix4x4& matrix);
int doNothing();

extern GLFWwindow* game_window;
const string LoadImage = "textures/please-stand-by.jpg";
const string LoadFile = "Load.file";

#endif