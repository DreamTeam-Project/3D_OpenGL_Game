#include "Light.h"

void GameLight::SetPointLights(const GameShader& shader) {
	string buf;
	for (uint i = 0; i < PointLights.size(); i++) {
		buf = string("pointLights[") + to_string(i) + string("].");
		shader.setVec3(buf + "position", PointLights[i].position + PointLights[i].model->place_);
		shader.setVec3(buf + "ambient", PointLights[i].ambient);
		shader.setVec3(buf + "diffuse", PointLights[i].diffuse);
		shader.setVec3(buf + "specular", PointLights[i].specular);
		shader.setFloat(buf + "constant", PointLights[i].constant);
		shader.setFloat(buf + "linear", PointLights[i].linear);
		shader.setFloat(buf + "quadratic", PointLights[i].quadratic);
	}
}

void GameLight::SetSpotLights(const GameShader& shader) {
	string buf;
	for (uint i = 0; i < SpotLights.size(); i++) {
		buf = string("spotLights[") + to_string(i) + string("].");
		shader.setVec3(buf + "position", SpotLights[i].position + SpotLights[i].model->place_);
		shader.setVec3(buf + "direction", SpotLights[i].direction/* * SpotLights[i].model->quat_*/);
		shader.setVec3(buf + "ambient", SpotLights[i].ambient);
		shader.setVec3(buf + "diffuse", SpotLights[i].diffuse);
		shader.setVec3(buf + "specular", SpotLights[i].specular);
		shader.setFloat(buf + "constant", SpotLights[i].constant);
		shader.setFloat(buf + "linear", SpotLights[i].linear);
		shader.setFloat(buf + "quadratic", SpotLights[i].quadratic);
	}
}

void GameLight::SetGlobalLight(const GameShader& shader) {
	shader.setVec3("dirLight.direction", Global.direction);
	shader.setVec3("dirLight.ambient", Global.ambient);
	shader.setVec3("dirLight.diffuse", Global.diffuse);
	shader.setVec3("dirLight.specular", Global.specular);
}

void GameLight::SetLight(const GameShader& shader) {
	shader.setInt("numPointLights", PointLights.size());
	shader.setInt("numSpotLights", SpotLights.size());
	SetGlobalLight(shader);
	SetPointLights(shader);
	SetSpotLights(shader);
}