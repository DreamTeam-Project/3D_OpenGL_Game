#ifndef LIGHT_H
#define LIGHT_H

#define GLEW_STATIC

#include <vector>
#include <string>

#include "Shader.h"
#include "Model.h"

using std::string;
using std::vector;

struct GlobalLight {
	GlobalLight(const vec3& dir = vec3(-0.2f, -1.0f, -0.3f),const vec3& amb = vec3(0.0f, 0.0f, 0.0f), 
		const vec3& diff = vec3(0.05f, 0.05f, 0.05f), const vec3& spec = vec3(0.1f, 0.1f, 0.1f)) :
		direction(dir), ambient(amb), diffuse(diff), specular(spec) {  }
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	PointLight() = delete;
	PointLight(GameModel* mod, const vec3& pos, const vec3& diff, const vec3& amb,
		const vec3& spec, float con, float lin, float quad) : 
	model(mod), position(pos), ambient(amb), diffuse(diff), 
		specular(spec), constant(con), quadratic(quad) {  }
	GameModel* model;
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	SpotLight() = delete;
	SpotLight(GameModel* mod, const vec3& pos, const vec3& diff, const vec3& amb,
		const vec3& dir, const vec3& spec, float con, float lin, float quad, float cut, float out) :
		model(mod), position(pos), direction(dir), ambient(amb), diffuse(diff), specular(spec), 
		constant(con), quadratic(quad), cutOff(glm::cos(glm::radians(cut))), outerCutOff(glm::cos(glm::radians(out))) 
	{  }
	GameModel* model;
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};

class GameLight {
public:
	GameLight(const GameLight&) = delete;
	GameLight(const GameLight&&) = delete;
	GameLight() : Global(GlobalLight()) {  }
	vector<PointLight> PointLights;
	vector<SpotLight> SpotLights;
	GlobalLight Global;
	void SetLight(const GameShader& shader);
private:
	void SetPointLights(const GameShader& shader);
	void SetSpotLights(const GameShader& shader);
	void SetGlobalLight(const GameShader& shader);
};

#endif