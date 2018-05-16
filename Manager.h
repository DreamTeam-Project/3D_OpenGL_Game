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
#include "Text.h"
#include "Skybox.h"
#include "Text.h"
#include "Image.h"
#include "Physics.h"

using std::vector;
using std::string;

struct Level {
	string name_;
	string pathLoader_;
	Level(string name = "default", string path = "default") : name_(name), pathLoader_(path) { }
};
struct LoadedModel final {
	LoadedModel(const string& path, int type, uint id)
		: path_(path), id_(id), type_(type) { }
	string path_;
	int type_;
	uint id_;
};

class GameManager {
public:
	GameManager();
	bool GameMenu(GLFWwindow *window);
	void EndLevel();
	void RenderWorld(const mat4& projection, const mat4& view, const Camera& camera, float time = 0);
	bool play;

	phys_world real_world_;
	phys_body camera_;

private:
	void MadeModels(const Init& init);
	void LoadModels();
	void RenderModels(const mat4& projection, const mat4& view, const Camera& camera, float time = 0);
	void LoadInfoAboutLevels();
	void LoadInfoAboutModels(uint levelNumber);
	int ChooseLevel() { return 0; }
	int ChooseLevel(GLFWwindow* window);
	void ProcessInputInMenu(GLFWwindow* window, uint& key_pressed);

	Image Loading;
	Skybox box;
	GameText text;
	GameLight Light;
	GameShader Shader;
	vector<Level> Levels;
	vector<SysStrings> SysText;
	vector<GameModel*> Models;
	vector<LoadedModel> LoadedModels;
};

#endif