#ifndef MODEL_H
#define MODEL_H

#define GLEW_STATIC

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image_aug.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/anim.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <irrKlang.h>

#include "Mesh.h"
#include "Shader.h"
#include "Physics.h"
#include "Sound.h"

using std::map;
using std::vector;
using std::string;
using glm::vec3;
using glm::vec2;
using glm::mat4;

#define ASSIMP_FLAGS aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs
uint TextureFromFile(const char *path, const string &directory);

class GameModel {
public:
	GameModel() = delete;
	explicit GameModel(const GameModel* model, const vec3& place, const vec3& quat, const vec3& scale, bool draw = false);
	GameModel(phys_body* psmodel, GameModel* grmodel, bool draw = false);
	explicit GameModel(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path, 
		const vec3& scale, const double& mass, const vec3& box, float shininess = 32.0f, bool draw = true);
	explicit GameModel(phys_world& real_world_, const vec3& place, const vec3& quat, const string& path,
		const vec3& scale, const double& mass, const vec3& box, float shininess = 32.0f, bool draw = true);
	void Draw(const GameShader& shader);
	void LoadModel();
	void CopyModel(const GameModel* model);
	void ClearLoaded();

	void SetShaderParameters(const GameShader& shader, float deltaTime);
	virtual void Move(mat4& model, float deltaTime);

	~GameModel() {
		delete rigid_body_;
	}

	string directory_;
	string path_;
	vec3 quat_;
	vec3 scale_;
	int type_;
	float shininess_;
	bool draw_;

	phys_body* rigid_body_;

private:
	vector<GameTexture> textures_loaded_;
	vector<Mesh*> meshes_;
	const aiScene* scene_;

	void ProcessNode(aiNode *node, const aiScene *scene);
	Mesh* ProcessMesh(aiMesh *mesh, const aiScene *scene, uint MeshIndex);
	vector<GameTexture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, const string& typeName);
};

class Structure : public GameModel {
public:
	Structure() = delete;
	Structure(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path,
		const vec3& scale, const double& mass, const vec3& box, float shininess, bool draw = true);
	Structure(phys_world& real_world_, const vec3& place, const vec3& quat, const string& path,
		const vec3& scale, const double& mass, const vec3& box, float shininess, bool draw = true) :
		GameModel(real_world_, place, quat, path, scale, mass, box, shininess, draw)
	{ 
		model_ = glm::translate(model_, place);
		model_ = glm::rotate(model_, glm::radians(quat.x), vec3(1.0f, 0.0f, 0.0f));
		model_ = glm::rotate(model_, glm::radians(quat.y), vec3(0.0f, 1.0f, 0.0f));
		model_ = glm::rotate(model_, glm::radians(quat.z), vec3(0.0f, 0.0f, 1.0f));
		model_ = glm::scale(model_, scale);
	}
	void Move(mat4& model, float deltaTime) override;
protected:
	mat4 model_;
};

class FloorModel : public Structure {
public:
	FloorModel(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path,
		const vec3& scale, const double& mass, const vec3& box, const vector<string>& sounds, irrklang::ISoundEngine* engine3d,
		map<string, irrklang::ISoundSource*> LoadedSounds, float shininess, bool draw = true) :
		Structure(real_world_, type, place, quat, path, scale, mass, box, shininess, draw),
		floor(a = irrklang::vec3df(place.x, place.y, place.z), sounds, PASSIVE_Sound, engine3d, LoadedSounds) { }
private:
	irrklang::vec3df a;
	SoundStructure floor;
	void Move(mat4& model, float deltaTime) override {
		model = model_;
		PlayMusic(deltaTime);
	}
	void PlayMusic(float deltaTime) {
		irrklang::vec3df tmp1 = irrklang::vec3df(rigid_body_->get_pos().x, rigid_body_->get_pos().y, rigid_body_->get_pos().z);
		irrklang::vec3df tmp2 = irrklang::vec3df(camera.position->get_pos().x, camera.position->get_pos().y, camera.position->get_pos().z);
		floor.Refresh(tmp1, tmp2, (StructureSound)rigid_body_->get_status());
	}
};

class CharacterModel : public GameModel {
public:
	CharacterModel(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path,
		const vec3& scale, const double& mass, const vec3& box, const vector<string>& sounds, irrklang::ISoundEngine* engine3d,
		map<string, irrklang::ISoundSource*> LoadedSounds, float shininess, bool draw = false) : 
		GameModel( real_world_, place, quat, path, scale, mass, box,shininess, true), 
		hero(a = irrklang::vec3df(place.x, place.y, place.z), sounds, ALIVE_Sound, engine3d, LoadedSounds) 
	{ 
		rigid_body_ = new Character(real_world_, btVector3(place.x, place.y, place.z), btVector3(box.x, box.y, box.z), btScalar(mass));
		camera.position = rigid_body_;
	}

	SoundHero hero;
	irrklang::vec3df a;
	void Move(mat4& model, float deltaTime) override {
		PlayMusic(deltaTime);
		model = glm::translate(model, rigid_body_->get_pos()
			+ vec3(camera.Front) * 0.3f + vec3(0, -1,0)
		);
		model = glm::rotate(model, (quat_.x), vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (quat_.y), vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (quat_.z), vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, scale_);
	}
	void PlayMusic(float deltaTime) {
		irrklang::vec3df tmp1 = irrklang::vec3df(rigid_body_->get_pos().x, rigid_body_->get_pos().y, rigid_body_->get_pos().z);
		hero.Refresh(tmp1, tmp1, (CharacterSound)rigid_body_->get_status(), deltaTime);
	}
};

class EnemyDisModel : public GameModel {
public:
	EnemyDisModel(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path,
		const vec3& scale, const double& mass, const vec3& box, const vector<string>& sounds, irrklang::ISoundEngine* engine3d,
		map<string, irrklang::ISoundSource*> LoadedSounds, float shininess, bool draw = true) :
		GameModel(real_world_, place, quat, path, scale, mass, box, shininess, draw),
		hero(a = irrklang::vec3df(place.x, place.y, place.z), sounds, ALIVE_Sound, engine3d, LoadedSounds)
	{
		rigid_body_ = new Enemy_dis(real_world_, btVector3(place.x, place.y, place.z), btVector3(box.x, box.y, box.z), btScalar(mass));
	}
	irrklang::vec3df a;
	void Move(mat4& model, float deltaTime) override {
		PlayMusic(deltaTime);
		model = glm::translate(model, rigid_body_->get_pos() - vec3(0.0f, 4.0f, 0.0f));
		quat_ = rigid_body_->get_quat();
		model = glm::rotate(model, quat_.x + glm::radians(270.0f), vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, quat_.y, vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, quat_.z, vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, scale_);
	}
	void PlayMusic(float deltaTime) {
		irrklang::vec3df tmp1 = irrklang::vec3df(rigid_body_->get_pos().x, rigid_body_->get_pos().y, rigid_body_->get_pos().z);
		irrklang::vec3df tmp2 = irrklang::vec3df(camera.position->get_pos().x, camera.position->get_pos().y, camera.position->get_pos().z);
		if(rigid_body_->get_status() == ATTACK_Sound)
			printf("shouting\n");
		hero.Refresh(tmp1, tmp2, (CharacterSound)rigid_body_->get_status());
	}
private:
	SoundCharacter hero;
};

class BulletModel : public GameModel {
public:
	BulletModel(phys_body* psmodel, GameModel* grmodel, bool draw) : GameModel(psmodel, grmodel, draw)
	{	 }
	BulletModel(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path,
		const vec3& scale, const double& mass, const vec3& box, float shininess, bool draw = true) :
		GameModel(real_world_, place, quat, path, scale, mass, box, shininess, draw) 
	{
		rigid_body_ = new Bullet(real_world_, btVector3(place.x, place.y, place.z), btVector3(box.x, box.y, box.z), btScalar(mass));
	}	
};

class XPBoxModel : public Structure {
public:
	XPBoxModel(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path,
		const vec3& scale, const double& mass, const vec3& box, float shininess, bool draw = true) : 
		Structure(real_world_, place, quat, path, scale, mass, box, shininess, draw) 
	{
		rigid_body_ = new  HP_box(real_world_, btVector3(place.x, place.y, place.z), btVector3(box.x, box.y, box.z), btScalar(mass));
	}
	void Move(mat4& model, float deltaTime) override {
		model = model_;

	}

};

class ChurchModel : public Structure {
public:
	vector<phys_body*> walls;
	ChurchModel(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path,
		const vec3& scale, const double& mass, const vector<vec3>& box, const vector<string>& sounds, irrklang::ISoundEngine* engine3d,
		map<string, irrklang::ISoundSource*> LoadedSounds, float shininess, bool draw = true) : 
		Structure(real_world_, place, quat, path, scale, mass, box[0], shininess, draw) 
	{
		rigid_body_ = new phys_body(real_world_, btVector3(place.x, place.y, place.z), btVector3(box[0].x, box[0].y, box[0].z), btScalar(0));
	}
	~ChurchModel() {
		int i = walls.size();
		while (i > 0) {
			delete walls[i - 1];
			i--;
		}

	}
};

class BulletBoxModel : public Structure {
public:
	BulletBoxModel(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path,
		const vec3& scale, const double& mass, const vec3& box, float shininess, bool draw = true) : 
		Structure(real_world_, place, quat, path, scale, mass, box, shininess, draw) 
	{
		rigid_body_ = new Box_bullet(real_world_, btVector3(place.x, place.y, place.z), btVector3(box.x, box.y, box.z), btScalar(mass));
	}
};

class EnemyCloseModel : public GameModel {
public:
	EnemyCloseModel(phys_world& real_world_, const int& type, const vec3& place, const vec3& quat, const string& path,
		const vec3& scale, const double& mass, const vec3& box, const vector<string>& sounds, irrklang::ISoundEngine* engine3d,
		map<string, irrklang::ISoundSource*> LoadedSounds, float shininess, bool draw = true) :
		GameModel(real_world_, place, quat, path, scale, mass, box, shininess, draw),
		hero(a = irrklang::vec3df(place.x, place.y, place.z), sounds, ALIVE_Sound, engine3d, LoadedSounds)
	{
		rigid_body_ = new Enemy_close(real_world_, btVector3(place.x, place.y, place.z), btVector3(box.x, box.y, box.z), btScalar(mass));
	}
	irrklang::vec3df a;
	void Move(mat4& model, float deltaTime) override {
		PlayMusic(deltaTime);
		quat_ = rigid_body_->get_quat();
		model = glm::translate(model, rigid_body_->get_pos() - vec3(0.0f, 4.0f, 0.0f));
		model = glm::rotate(model, quat_.x, vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, quat_.y, vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, quat_.z, vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, scale_);
	}
	void PlayMusic(float deltaTime) {
		irrklang::vec3df tmp1 = irrklang::vec3df(rigid_body_->get_pos().x, rigid_body_->get_pos().y, rigid_body_->get_pos().z);
		irrklang::vec3df tmp2 = irrklang::vec3df(camera.position->get_pos().x, camera.position->get_pos().y, camera.position->get_pos().z);
		hero.Refresh(tmp1, tmp2, (CharacterSound)rigid_body_->get_status());
	}
private:
	SoundCharacter hero;
};

#endif