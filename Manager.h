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

using std::vector;
using std::string;
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
			throw GameException(__LINE__, __func__, "error push back level");
		}
	}
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
	void RenderModels(const GameShader& shader);
	void LoadInfoAboutLevels();
	void LoadInfoAboutModels(uint levelNumber);
	uint ChooseLevel() { return 0; }
	bool BeOrNotToBe() { return false; }

private:
	void LoadModels();
	Levels levels;;
	GameLight Light;
	vector<GameModel*> AllModels;
	vector<LoadedModel> LoadedModels;
};

#endif