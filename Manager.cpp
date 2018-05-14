#include "Manager.h"

GameManager::GameManager() : 
	/*text(FontFile),*/ /*box(DarkStormy, lightSky),*/ Shader("Light.vs", "Light.fs"), AniShader("Skinning.vs", "Light.fs"),
	play(false)/*, real_world_(phys_world())*/ {
	LoadInfoAboutLevels();
}

void GameManager::LoadInfoAboutLevels() {
	std::ifstream fin(LoadFile);
	if (!fin.is_open()) {
		throw GameException(__LINE__, __func__, "error open file Load.file");
	}
	string name_buf = "";
	string path_buf = "";
#if DEBUG_MANAGER
	print(string("\ntry to read: ") + LoadFile);
#endif
	while (!fin.eof()) {
		getStringFromFile(fin, name_buf);
		if (name_buf == "end_of_file" || name_buf == "null") {
			break;
		}
		getStringFromFile(fin, path_buf);
		if (path_buf == "end_of_file" || name_buf == "null") {
			throw GameException(__LINE__, __func__, "error format Load.file");
		}
		Levels.push_back(Level(name_buf, path_buf));
	}
	if (Levels.size() == 0) {
		throw GameException(__LINE__, __func__, "LoadFile.file is empty");
	}
#if DEBUG_MANAGER
	print("success\n");
#endif
}

void GameManager::LoadInfoAboutModels(uint levelNumber) {
	string path_tmp = Levels.at(levelNumber).pathLoader_;
	std::ifstream fin(path_tmp);
	if (!fin.is_open()) {
		throw GameException(__LINE__, __func__, "error open file level.file");
	}

#if DEBUG_MANAGER
	print(string("try to read: ") + path);
#endif
	int type;
	vec3 place;
	vec3 quat;
	string path;
	vec3 scale;
	double mass;
	vec3 box;
	while (!fin.eof()) {
		GameModel* NewModel = nullptr;
		string strbuf = "";
		getStringFromFile(fin, strbuf);
		if (strbuf == "null") {
			throw GameException(__LINE__, __func__, "error level.path");
		}
		if (strbuf == "end_of_file") {
			break;
		}
		if (strbuf != "type") {
			throw GameException(__LINE__, __func__, "error level.path");
		}

		getStringFromFile(fin, type);
		if (strbuf == "null" || strbuf == "end_of_file") {
			throw GameException(__LINE__, __func__, "error level.path");
		}

		getStringFromFile(fin, strbuf);
		if (strbuf != "path" || strbuf == "end_of_file") {
			throw GameException(__LINE__, __func__, "error type");
		}
		getStringFromFile(fin, path);

		getStringFromFile(fin, strbuf);
		if (strbuf != "place" || strbuf == "end_of_file") {
			throw GameException(__LINE__, __func__, "error place");
		}
		getStringFromFile(fin, place);

		getStringFromFile(fin, strbuf);
		if (strbuf != "quat" || strbuf == "end_of_file") {
			throw GameException(__LINE__, __func__, "error quat");
		}
		getStringFromFile(fin, quat);

		getStringFromFile(fin, strbuf);
		if (strbuf != "scale" || strbuf == "end_of_file") {
			throw GameException(__LINE__, __func__, "error scale");
		}
		getStringFromFile(fin, scale);

		getStringFromFile(fin, strbuf);
		if (strbuf != "mass" || strbuf == "end_of_file") {
			throw GameException(__LINE__, __func__, "error mass");
		}
		getStringFromFile(fin, mass);

		getStringFromFile(fin, strbuf);
		if (strbuf != "box" || strbuf == "end_of_file") {
			throw GameException(__LINE__, __func__, "error box");
		}
		getStringFromFile(fin, box);
		if (!NewModel) {
			switch (type) {
			case GAMEMODEL:
				NewModel = new GameModel(/*real_world_, */type, place, quat, path, scale, mass, box, 32.0f, true);
				break;
			case ANIMATION:
				NewModel = new AnimatedModel(/*real_world_, */type, place, quat, path, scale, mass, box, 32.0f, true);
				break;
			case STRUCTURE:
				NewModel = new Structure(/*real_world_, */type, place, quat, path, scale, mass, box, 16.0f, true);
				break;
			case STREETLAMP:
				NewModel = new StreetLamp(/*real_world_, */type, place, quat, path, scale, mass, box, 32.0f, true, true);
				break;
			default:
				NewModel = new GameModel(/*real_world_, */type, place, quat, path, scale, mass, box, 32.0f, true);
			}
		}
		else {
			throw GameException(__LINE__, __func__, "Error wrong type of class");
		}
#if DEBUG_MANAGER
		NewModel->PrintModel();
#endif
		if (NewModel->type_ != ANIMATION) {
			Models.push_back(NewModel);
		}
		else {
			AniModels.push_back(NewModel);
		}
	}
	if (Models.size() == 0 && AniModels.size()) {
		throw GameException(__LINE__, __func__, "level.file is empty");
	}
	LoadModels(Models);
	LoadModels(AniModels);
#if DEBUG_MANAGER
	print("success");
#endif
}

void GameManager::LoadModels(const vector<GameModel*>& model) {
	bool ModelLoaded = false;
	for (uint i = 0; i < model.size(); i++) {
		ModelLoaded = false;
		for (auto& it : LoadedModels) {
			if (it.path_ == model[i]->path_ && it.type_ == model[i]->type_) {
				model[i]->CopyModel(model[it.id_]);
				ModelLoaded = true;
			}
		}
		if (!ModelLoaded) {
			LoadedModels.push_back(LoadedModel(model[i]->path_, model[i]->type_, i));
			model[i]->LoadModel();
		}
	}
	for (auto& it : model) {
		it->ClearLoaded();
	}
}

void GameManager::RenderModels(const mat4& projection, const mat4& view, const Camera& camera, const GameShader& shader, const vector<GameModel*>& model) {
	shader.Use();
	Light.SetLight(shader);
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setVec3("viewPos", camera.Position);
	for (auto& it : model) {
		if (it->draw_) {
			it->SetShaderParameters(shader);
			it->Draw(shader);
		}
	}
}

bool GameManager::GameMenu(GLFWwindow* window/*, const Image& Loading*/) {
#if DEBUG_MENU
	int levelNumber = ChooseLevel();
#else
	int levelNumber = ChooseLevel(window);
#endif
	if (levelNumber == -1) {
		return false;
	}
	else {
		//Loading.RenderImage(true);
		LoadInfoAboutModels(levelNumber);
		play = true;
		return true;
	}
}

void GameManager::RenderWorld(const mat4& projection, const mat4& view, const Camera& camera, float time) {
	//real_world_.do_step(time);
	RenderModels(projection, view, camera, Shader, Models);
	RenderModels(projection, view, camera, AniShader, AniModels);
	//box.RenderBox(camera, projection);
	/*text.RenderText(SysText);*/
}

void GameManager::ProcessInputInMenu(GLFWwindow* window, uint& key_pressed) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		key_pressed = 1;
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		key_pressed = 3;
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		key_pressed = 2;
		return;
	}
}

void GameManager::EndLevel() {
	/*SysText.clear();*/
	Models.clear();
	AniModels.clear();
	LoadedModels.clear();
}

//int GameManager::ChooseLevel(GLFWwindow* window) {
//	uint key = 0;
//	uint chosen = 1;
//	GLdouble deltaTime, currentFrame, lastFrame = 0.0;
//	GLdouble accumulator = 0.0;
//	Image Menu(MenuImage);
//	SysText.clear();
//
//	SysText.push_back(SysStrings("Wastelands of the USSR", (float)HEIGHT / 4.5f, (float)WIDTH / 7 * 6, 0.85f, vec3(0.5f)));
//	for (uint i = 0; i < Levels.size(); i++) {
//		SysText.push_back(SysStrings(Levels[i].name_, (float)HEIGHT / 6, (float)WIDTH / 14 * (11 - (i + 1)), 0.7f, vec3(0.5f)));
//	}
//
//	SysText[chosen].color_ = vec3(1.0f, 0.0f, 0.0f);
//
//	while (!glfwWindowShouldClose(game_window)) {
//		currentFrame = glfwGetTime();
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//
//		accumulator += deltaTime;
//		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		ProcessInputInMenu(game_window, key);
//
//		if (accumulator > 0.5) {
//			accumulator = 0;
//			if (key == 3) {
//				if (chosen < Levels.size()) {
//					SysText[chosen].color_ = vec3(0.5f);
//					chosen++;
//					SysText[chosen].color_ = vec3(1.0f, 0.0f, 0.0f);
//				}
//				else {
//					SysText[chosen].color_ = vec3(0.5f);
//					chosen = 1;
//					SysText[chosen].color_ = vec3(1.0f, 0.0f, 0.0f);
//				}
//			}
//			else if (key == 1) {
//				if (chosen > 1) {
//					SysText[chosen].color_ = vec3(0.5f);
//					chosen--;
//					SysText[chosen].color_ = vec3(1.0f, 0.0f, 0.0f);
//				}
//				else {
//					SysText[chosen].color_ = vec3(0.5f);
//					chosen = Levels.size();
//					SysText[chosen].color_ = vec3(1.0f, 0.0f, 0.0f);
//				}
//			}
//			else if (key == 2) {
//				return chosen - 1;
//			}
//			key = 0;
//		}
//
//		Menu.RenderImage();
//		text.RenderText(SysText);
//
//		glfwSwapBuffers(game_window);
//		glfwPollEvents();
//	}
//
//	SysText.clear();
//	return -1;
//}