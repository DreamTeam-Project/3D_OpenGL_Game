#include "Manager.h"

GameManager::GameManager() : 
	Loading(LoadImage, true), text(FontFile), box(DarkStormy, lightSky), Shader("Light.vs", "Light.fs"), play(false), real_world_(phys_world()) {
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

void GameManager::MadeModels(const Init& init) {
	for (uint i = 0; i < init.size_phys; i++) {
		GameModel* NewModel = nullptr;
		switch (init.type) {
		case GAMEMODEL:
			NewModel = new GameModel(real_world_, init.type, init.place[i], init.quat[i], init.path, init.scale[i], init.mass, init.box, 32.0f, true);
			break;
		case ANIMATION:
			NewModel = new AnimatedModel(real_world_, init.type, init.place[i], init.quat[i], init.path, init.scale[i], init.mass, init.box, 32.0f, true);
			break;
		case STRUCTURE:
			NewModel = new Structure(real_world_, init.type, init.place[i], init.quat[i], init.path, init.scale[i], init.mass, init.box, 16.0f, true);
			break;
		case STREETLAMP:
			NewModel = new StreetLamp(real_world_, init.type, init.place[i], init.quat[i], init.path, init.scale[i], init.mass, init.box, 32.0f, true, true);
			//Light.PointLights.push_back(PointLight(NewModel, ))
		default:
			NewModel = new GameModel(real_world_, init.type, init.place[i], init.quat[i], init.path, init.scale[i], init.mass, init.box, 32.0f, true);
		}
		Models.push_back(NewModel);
	}
}

void GameManager::LoadInfoAboutModels(uint levelNumber) {
	string path_tmp = Levels.at(levelNumber).pathLoader_;
	std::ifstream fin(path_tmp);
	if (!fin.is_open()) {
		throw GameException(__LINE__, __func__, "error open file level.file");
	}
	while (!fin.eof()) {
		Init buf;
		string strbuf = "";

		getStringFromFile(fin, strbuf);
		if (strbuf == "end_of_file") {
			break;
		}
		if (strbuf == "null" || strbuf != "type") {
			throw GameException(__LINE__, __func__, "error level.path");
		}
		getStringFromFile(fin, buf.type);
		if (strbuf == "null" || strbuf == "end_of_file") {
			throw GameException(__LINE__, __func__, "error level.path");
		}

		ReadFromFile(fin, "path", buf.path);
		ReadFromFile(fin, "mass", buf.mass);
		ReadFromFile(fin, "box", buf.box);
		ReadFromFile(fin, "num", buf.size_phys);
		if (buf.size_phys <= 0) {
			throw GameException(__LINE__, __func__, "error level.path");
		}
		buf.place.resize(buf.size_phys);
		buf.scale.resize(buf.size_phys);
		buf.quat.resize(buf.size_phys);
		for (uint i = 0; i < buf.size_phys; i++) {
			ReadFromFile(fin, "place", buf.place[i]);
			ReadFromFile(fin, "quat", buf.quat[i]);
			ReadFromFile(fin, "scale", buf.scale[i]);
		}
		ReadFromFile(fin, "num", buf.size_sound);
		if (buf.size_sound < 0) {
			throw GameException(__LINE__, __func__, "error level.path");
		}
		buf.sound.resize(buf.size_sound);
		buf.type_1.resize(buf.size_sound);
		buf.type_2.resize(buf.size_sound);
		for (uint i = 0; i < buf.size_sound; i++) {
			ReadFromFile(fin, "sound", buf.sound[i]);
			ReadFromFile(fin, "yes", buf.type_1[i]);
			ReadFromFile(fin, "no", buf.type_2[i]);
		}
		MadeModels(buf);
	}
	if (Models.size() == 0) {
		throw GameException(__LINE__, __func__, "level.file is empty");
	}
	LoadModels();
}

void GameManager::RenderModels(const mat4& projection, const mat4& view, const Camera& camera, float time) {
	Shader.Use();
	Light.SetLight(Shader);
	Shader.setMat4("projection", projection);
	Shader.setMat4("view", view);
	Shader.setVec3("viewPos", camera.Position);
	for (auto& it : Models) {
		if (it->draw_) {
			it->SetShaderParameters(Shader);
			it->Draw(Shader);
		}
	}
}

bool GameManager::GameMenu(GLFWwindow* window) {
#if DEBUG_MENU
	int levelNumber = ChooseLevel();
#else
	int levelNumber = ChooseLevel(window);
#endif
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
	real_world_.do_step(time);
	RenderModels(projection, view, camera, time);
	box.RenderBox(camera, projection);
	text.RenderText("HP, MP bullets", 10.0f, (float)WIDTH / 2, 1.0f, vec3(1.0f, 0.0f, 0.0f));
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
	Loading.RenderImage(true);
	SysText.clear();
	Models.clear();
	LoadedModels.clear();
}

void GameManager::LoadModels() {
	bool ModelLoaded = false;
	for (uint i = 0; i < Models.size(); i++) {
		ModelLoaded = false;
		for (auto& it : LoadedModels) {
			if (it.path_ == Models[i]->path_ && it.type_ == Models[i]->type_) {
				Models[i]->CopyModel(Models[it.id_]);
				ModelLoaded = true;
			}
		}
		if (!ModelLoaded) {
			LoadedModels.push_back(LoadedModel(Models[i]->path_, Models[i]->type_, i));
			Models[i]->LoadModel();
		}
	}
	for (auto& it : Models) {
		it->ClearLoaded();
	}
}

int GameManager::ChooseLevel(GLFWwindow* window) {
	uint key = 0;
	uint chosen = 1;
	GLdouble deltaTime, currentFrame, lastFrame = 0.0;
	GLdouble accumulator = 0.0;
	Image Menu(MenuImage);
	SysText.clear();

	SysText.push_back(SysStrings(GameName, (float)HEIGHT / 4.5f, (float)WIDTH / 7 * 6, 0.85f, vec3(0.5f)));
	for (uint i = 0; i < Levels.size(); i++) {
		SysText.push_back(SysStrings(Levels[i].name_, (float)HEIGHT / 6, (float)WIDTH / 14 * (11 - (i + 1)), 0.7f, vec3(0.5f)));
	}

	SysText[chosen].color_ = vec3(1.0f, 0.0f, 0.0f);

	while (!glfwWindowShouldClose(game_window)) {
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		accumulator += deltaTime;
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ProcessInputInMenu(game_window, key);

		if (accumulator > 0.6) {
			accumulator = 0;
			if (key == 3) {
				if (chosen < Levels.size()) {
					SysText[chosen].color_ = vec3(0.5f);
					chosen++;
					SysText[chosen].color_ = vec3(1.0f, 0.0f, 0.0f);
				}
				else {
					SysText[chosen].color_ = vec3(0.5f);
					chosen = 1;
					SysText[chosen].color_ = vec3(1.0f, 0.0f, 0.0f);
				}
			}
			else if (key == 1) {
				if (chosen > 1) {
					SysText[chosen].color_ = vec3(0.5f);
					chosen--;
					SysText[chosen].color_ = vec3(1.0f, 0.0f, 0.0f);
				}
				else {
					SysText[chosen].color_ = vec3(0.5f);
					chosen = Levels.size();
					SysText[chosen].color_ = vec3(1.0f, 0.0f, 0.0f);
				}
			}
			else if (key == 2) {
				return chosen - 1;
			}
			key = 0;
		}

		Menu.RenderImage();
		text.RenderText(SysText);

		glfwSwapBuffers(game_window);
		glfwPollEvents();
	}

	SysText.clear();
	return -1;
}