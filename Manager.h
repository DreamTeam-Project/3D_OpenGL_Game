#ifndef MANAGER_H
#define MANAGER_H

#define GLEW_STATIC

#include <vector>
#include <string>

#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "System.h"
#include "Light.h"
#include "Camera.h"
//#include "Text.h"
#include "Skybox.h"
#include "Image.h"
//#include "Physics.h"

using std::vector;
using std::string;

struct Level {
	string name_;
	string pathLoader_;
	Level(string name = "default", string path = "default") : name_(name), pathLoader_(path) { }
};
struct LoadedModel {
	LoadedModel(const string& path, int type, uint id)
		: path_(path), id_(id), type_(type) { }
	string path_;
	int type_;
	uint id_;
};

class GameManager {
public:
	GameManager();
	bool GameMenu(GLFWwindow *window, const Image& Loading);
	void EndLevel();
	void RenderWorld(const mat4& projection, const mat4& view, const Camera& camera, float time = 0);
	bool play;

	//phys_world real_world_;
	//phys_body camera_;

private:
	void RenderModels(const mat4& projection, const mat4& view, const Camera& camera, const GameShader& shader, const vector<GameModel*>& model);
	void LoadInfoAboutLevels();
	void LoadInfoAboutModels(uint levelNumber);
	int ChooseLevel() { return 0; }
	//int ChooseLevel(GLFWwindow* window);
	void ProcessInputInMenu(GLFWwindow* window, uint& key_pressed);
	void LoadModels(const vector<GameModel*>& model);

	Skybox box;
	//GameText text;
	GameLight Light;
	GameShader Shader;
	GameShader AniShader;
	vector<Level> Levels;
	//vector<SysStrings> SysText;
	vector<GameModel*> Models;
	vector<GameModel*> AniModels;
	vector<LoadedModel> LoadedModels;
};

#endif