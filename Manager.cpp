#include "Manager.h"

GameManager::GameManager() : text(FontFile), box(DarkStormy, lightSky), Shader("Light.vs", "Light.fs"), play(false) {
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
			throw GameException(__LINE__, __func__, "error format Load.file");
		}
		getStringFromFile(fin, path_buf);
		if (path_buf == "end_of_file" || name_buf == "null") {
			break;
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
	string path = Levels.at(levelNumber).pathLoader_;
	std::ifstream fin(path);
	if (!fin.is_open()) {
		throw GameException(__LINE__, __func__, "error open file level.file");
	}

#if DEBUG_MANAGER
	print(string("try to read: ") + path);
#endif
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

		int type = 0;
		getStringFromFile(fin, type);
		if (strbuf == "null" || strbuf == "end_of_file") {
			throw GameException(__LINE__, __func__, "error level.path");
		}
		if (!NewModel) {
			switch (type) {
			case GAMEMODEL:
				NewModel = new GameModel(32.0f, true);
				break;
			case ANIMATION:
				NewModel = new AnimatedModel(32.0f, true);
				break;
			case STRUCTURE:
				NewModel = new Structure(16.0f, true);
				break;
			case STREETLAMP:
				NewModel = new StreetLamp(32.0f, true, true);
				//Light.PointLights.push_back(PointLight(NewModel, ))
			default:
				NewModel = new GameModel(32.0f, true);
			}
			NewModel->type_ = type;
		}
		else {
			throw GameException(__LINE__, __func__, "Error wrong type of class");
		}

		getStringFromFile(fin, strbuf);
		if (strbuf != "path" || strbuf == "end_of_file") {
			throw GameException(__LINE__, __func__, "error type");
		}
		getStringFromFile(fin, NewModel->path_);

		getStringFromFile(fin, strbuf);
		if (strbuf != "place" || strbuf == "end_of_file") {
			throw GameException(__LINE__, __func__, "error place");
		}
		getStringFromFile(fin, NewModel->place_);;

		getStringFromFile(fin, strbuf);
		if (strbuf != "quat" || strbuf == "end_of_file") {
			throw GameException(__LINE__, __func__, "error quat");
		}
		getStringFromFile(fin, NewModel->quat_);

		getStringFromFile(fin, strbuf);
		if (strbuf != "scale" || strbuf == "end_of_file") {
			throw GameException(__LINE__, __func__, "error scale");
		}
		getStringFromFile(fin, NewModel->scale_);
#if DEBUG_MANAGER
		NewModel->PrintModel();
#endif
		AllModels.push_back(NewModel);
	}
	if (AllModels.size() == 0) {
		throw GameException(__LINE__, __func__, "level.file is empty");
	}
	LoadModels();
#if DEBUG_MANAGER
	print("success");
#endif
}

void GameManager::LoadModels() {
	bool ModelLoaded = false;
	for (uint i = 0; i < AllModels.size(); i++) {
		ModelLoaded = false;
		for (auto& it : LoadedModels) {
			if (it.path_ == AllModels[i]->path_ && it.type_ == AllModels[i]->type_) {
				AllModels[i]->CopyModel(AllModels[it.id_]);
				ModelLoaded = true;
			}
		}
		if (!ModelLoaded) {
			LoadedModels.push_back(LoadedModel(AllModels[i]->path_, AllModels[i]->type_, i));
			AllModels[i]->LoadModel();
		}
	}
	for (auto& it : AllModels) {
		it->ClearLoaded();
	}
}

void GameManager::RenderModels(const mat4& projection, const mat4& view, const Camera& camera, float time) {
	Shader.Use();
	Light.SetLight(Shader);
	Shader.setMat4("projection", projection);
	Shader.setMat4("view", view);
	Shader.setVec3("viewPos", camera.Position);
	for (auto& it : AllModels) {
		if (it->draw_) {
			it->SetShaderParameters(Shader);
			it->Draw(Shader);
		}
	}
}

bool GameManager::GameMenu(GLFWwindow* window, const Image& Loading) {
	//int levelNumber = ChooseLevel();
	int levelNumber = ChooseLevel(window);
	if (levelNumber == -1) {
		return false;
	}
	else {
		Loading.RenderImage(true);
		LoadInfoAboutModels(levelNumber);
		play = true;
		return true;
	}
}

void GameManager::RenderWorld(const mat4& projection, const mat4& view, const Camera& camera, float time) {
	RenderModels(projection, view, camera, time);
	box.RenderBox(camera, projection);
	text.RenderText("HP, MP bullets", 10.0f, (float)WIDTH / 2, 1.0f, vec3(1.0f, 0.0f, 0.0f));
	text.RenderText("HP, MP bullets", 10.0f, (float)WIDTH / 4, 1.0f, vec3(1.0f, 0.0f, 0.0f));
}

void GameManager::ProcessInputInMenu(GLFWwindow* window, uint& key_pressed) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	for (uint key = GLFW_KEY_1; key <= GLFW_KEY_9; key++) {
		if (glfwGetKey(window, key) == GLFW_PRESS) {
			key_pressed = key - GLFW_KEY_0;
			return;
		}
	}
}

void GameManager::EndLevel() {
	SysText.clear();
	AllModels.clear();
	LoadedModels.clear();
}

int GameManager::ChooseLevel(GLFWwindow* window) {
	uint key = 0;
	Image Menu(MenuImage);
	SysText.clear();
	for (uint i = 0; i < Levels.size(); i++) {
		SysText.push_back(SysStrings(to_string(i) + ":", 50.0f, 2.0f, 1.0f));
		SysText.push_back(SysStrings(Levels[i].name_, 60.0f * i, 10.0f, 1.0f));
	}

	while (!glfwWindowShouldClose(game_window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ProcessInputInMenu(game_window, key);

		Menu.RenderImage();

		if (0 < key && key <= Levels.size()) {
			return key - 1;
		}
		else {
			text.RenderText("Error button!", (float)WIDTH / 2, (float)HEIGHT / 2, 1.0f);
		}
		text.RenderText(SysText);
		
		glfwSwapBuffers(game_window);
		glfwPollEvents();
	}
	SysText.clear();
	return -1;
}