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

struct Level final {
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
struct Init final {
	int type;
	double mass;
	string path;

	int size_phys;
	vector<vec3> place;
	vector<vec3> quat;
	vector<vec3> scale;

	int size_box;
	vector<vec3> box;

	int size_sound;
	vector<string> sound;
	vector<int> type_1;
	vector<int> type_2;
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