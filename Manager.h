#ifndef MANAGER_H
#define MANAGER_H

#define GLEW_STATIC

#include <vector>
#include <string>
#include <map>
#include <irrKlang.h>

#include "Sound.h"
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
using std::map;

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
struct Unit final {
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
	~Unit() = default;
};
struct Union final {
	int num;
	vector<vec3> place;
	vector<vec3> quat;
	vector<Union*> unions;
	vector<Unit*> units;
	~Union() {
		for (auto& it : unions) {
			it->~Union();
		}
		for (auto& it : units) {
			it->~Unit();
		}
	}
};

class GameManager {
public:
	GameManager();
	~GameManager() {
		music->drop();
	}
	bool GameMenu(GLFWwindow *window);
	void EndLevel();
	void RenderWorld(const mat4& projection, const mat4& view, const Camera& camera, float time = 0);
	void GetCopy(GameModel* model, phys_body* phmodel);
	bool play;

	phys_world real_world_;
	phys_body camera_;
	irrklang::ISoundEngine* engine3d;

private:
	void LoadSound(ifstream& fin);
	Unit* LoadUnit(ifstream& fin);
	Union* LoadUnion(ifstream& fin);
	void MadeModels(Unit* init);
	void MadeModels(Unit* init, const vec3& place, const vec3& quat);
	void MadeModels(const int& type, const vec3& place, const vec3& quat, const string& path,
		const vec3& scale, const double& mass, const vector<vec3>& box, const vector<string>& sounds);
	void MadeModels(Union* init);
	void MadeModels(Union* init, const vec3& place, const vec3& quat);
	void LoadModels();
	void RenderModels(const mat4& projection, const mat4& view, const Camera& camera, float time = 0);
	void LoadInfoAboutLevels();
	void LoadInfoAboutModels(uint levelNumber);
	int ChooseLevel() { return 0; }
	int ChooseLevel(GLFWwindow* window);
	void ProcessInputInMenu(GLFWwindow* window, uint& key_pressed);

	irrklang::ISound* music;
	Image Loading;
	Skybox box;
	GameText text;
	GameLight Light;
	GameShader Shader;
	irrklang::ISoundSource* menuSound;
	vector<Level> Levels;
	vector<SysStrings> SysText;
	vector<GameModel*> Models;
	vector<LoadedModel> LoadedModels;
	map<string, irrklang::ISoundSource*> LoadedSounds;
};

#endif