#include "Manager.h"

extern Camera camera;

GameManager::GameManager() : 
	Loading(LoadImage, true), text(FontFile), box(DarkStormy, lightSky), Shader("Light.vs", "Light.fs"), 
	play(false), real_world_(phys_world())
{
	engine3d = irrklang::createIrrKlangDevice();
	menuSound = engine3d->addSoundSourceFromFile(MenuSound.c_str(), irrklang::ESM_STREAMING, true);
	music = engine3d->play3D(menuSound, irrklang::vec3df(0.0f, 0.0f, 0.0f), true, false, true);
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

void GameManager::MadeModels(Unit* init, const vec3& place, const vec3& quat) {
	for (int i = 0; i < init->size_phys; i++) {
		MadeModels(init->type, place + init->place[i], quat + init->quat[i], init->path, init->scale[i], init->mass, init->box, init->sound);
	}
}

void GameManager::MadeModels(Unit* init) {
	for (int i = 0; i < init->size_phys; i++) {
		MadeModels(init->type, init->place[i], init->quat[i], init->path, init->scale[i], init->mass, init->box, init->sound);
	}
}

void GameManager::MadeModels(const int& type, const vec3& place, const vec3& quat, const string& path, 
	const vec3& scale, const double& mass, const vector<vec3>& box, const vector<string>& sounds) 
{
	GameModel* NewModel = nullptr;
	switch (type) {
	case GAMEMODEL:
		NewModel = new GameModel(real_world_, type, place, quat, path, scale, mass, box[0], 32.0f, true);
		break;
	case CHARACTER:
		NewModel = new CharacterModel(real_world_, type, place, quat, path, scale, mass, box[0], sounds, engine3d , LoadedSounds, 32.0f, true);
		Light.SpotLights.push_back(SpotLight(NewModel, vec3(0.0f, 4.0f, 0.0f), vec3(0.8f, 0.8f, 0.8f), vec3(0.1f, 0.1f, 0.1f), camera.Front, vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, 12.5f, 17.5f));
		break;
	case STRUCTURE:
		NewModel = new Structure(real_world_, type, place, quat, path, scale, mass, box[0], 16.0f, true);
		break;
	case BULLET:
		NewModel = new BulletModel(real_world_, type, place, quat, path, scale, mass, box[0], 32.0f, true);
		break;
	case BOX_BULLET:
		NewModel = new BulletBoxModel(real_world_, type, place, quat, path, scale, mass, box[0], 32.0f, true);
		break;
	case FLOOR:
		NewModel = new FloorModel(real_world_, type, place, quat, path, scale, mass, box[0], sounds, engine3d, LoadedSounds, 8.0f, true);
		break;
	case ENEMY_CLOSE:
		NewModel = new EnemyCloseModel(real_world_, type, place, quat, path, scale, mass, box[0], sounds, engine3d, LoadedSounds, 32.0f, true);
		break;
	case ENEMY_DIS:
		NewModel = new EnemyDisModel(real_world_, type, place, quat, path, scale, mass, box[0], sounds, engine3d, LoadedSounds, 32.0f, true);
		break;
	case HP_BOX:
		NewModel = new XPBoxModel(real_world_, type, place, quat, path, scale, mass, box[0], 32.0f, true);
		break;
	case CHURCH:
		NewModel = new ChurchModel(real_world_, type, place, quat, path, scale, mass, box, sounds, engine3d, LoadedSounds, 16.0f, true);
		break;
	default:
		NewModel = new GameModel(real_world_, type, place, quat, path, scale, mass, box[0], 32.0f, true);
	}
	Models.push_back(NewModel);
}

void GameManager::MadeModels(Union* init) {
	for (int j = 0; j < init->num; j++) {
		for (uint i = 0; i < init->unions.size(); i++) {
			MadeModels(init->unions[i], init->place[j], init->quat[j]);
		}
		for (uint i = 0; i < init->units.size(); i++) {
			MadeModels(init->units[i], init->place[j], init->quat[j]);
		}
	}
}

void GameManager::MadeModels(Union* init, const vec3& place, const vec3& quat) {
	for (int j = 0; j < init->num; j++) {
		for (uint i = 0; i < init->unions.size(); i++) {
			MadeModels(init->unions[i], init->place[j] + place, init->quat[j] + quat);
		}
		for (uint i = 0; i < init->units.size(); i++) {
			MadeModels(init->units[i], init->place[j] + place, init->quat[j] + quat);
		}
	}
}

void GameManager::LoadSound(ifstream& fin) {
	string buf;
	int type;
	irrklang::ISoundSource* tmp = nullptr;
	getStringFromFile(fin, buf);
	if (buf == "null" || buf == "end_of_file" || buf != "sound") {
		throw GameException(__LINE__, __func__, "Error: level.path - sound module is bad");
	}
	bool flag = true;
	while (flag) {
		getStringFromFile(fin, buf);
		if (buf == "null" || buf == "end_of_file") {
			throw GameException(__LINE__, __func__, "Error: level.path - sound module is bad");
		}
		if (buf == "end") {
			flag = false;
			break;
		}
		getStringFromFile(fin, type);
		switch (type) {
		case 0: 
			tmp = engine3d->addSoundSourceFromFile(buf.c_str(), irrklang::ESM_NO_STREAMING, true);
			break;
		case 1:
			tmp = engine3d->addSoundSourceFromFile(buf.c_str(), irrklang::ESM_STREAMING, true);
			break;
		default:
			tmp = engine3d->addSoundSourceFromFile(buf.c_str(), irrklang::ESM_AUTO_DETECT, true);
			break;
		}
		auto it = LoadedSounds.end();
		if (LoadedSounds.find(buf) != it) {
			doNothing();
		}
		else {
			LoadedSounds.insert(std::make_pair(buf, tmp));
		}
	}
}

void GameManager::GetCopy(int num, phys_body* phmodel) {
	GameModel* tmp = new GameModel(phmodel, Models[num], true);
	Models.push_back(tmp);
}

Unit* GameManager::LoadUnit(ifstream& fin) {
	Unit* buf = new Unit();
	string strbuf = "";

	ReadFromFile(fin, "type", buf->type);
	ReadFromFile(fin, "path", buf->path);
	ReadFromFile(fin, "mass", buf->mass);
	ReadFromFile(fin, "num", buf->size_phys);

	if (buf->size_phys <= 0) {
		throw GameException(__LINE__, __func__, "Error: error level.path - size 0f phys param");
	}
	buf->place.resize(buf->size_phys);
	buf->scale.resize(buf->size_phys);
	buf->quat.resize(buf->size_phys);
	for (int i = 0; i < buf->size_phys; i++) {
		ReadFromFile(fin, "place", buf->place[i]);
		ReadFromFile(fin, "quat", buf->quat[i]);
		ReadFromFile(fin, "scale", buf->scale[i]);
	}

	ReadFromFile(fin, "num", buf->size_box);
	if (buf->size_box <= 0) {
		throw GameException(__LINE__, __func__, "Error: error level.path - num of boxes");
	}
	buf->box.resize(buf->size_box);
	for (int i = 0; i < buf->size_box; i++) {
		ReadFromFile(fin, "box", buf->box[i]);
	}

	ReadFromFile(fin, "num", buf->size_sound);
	if (buf->size_sound < 0) {
		throw GameException(__LINE__, __func__, "Error: error level.path - read num of sound");
	}
	buf->sound.resize(buf->size_sound);
	for (int i = 0; i < buf->size_sound; i++) {
		ReadFromFile(fin, "sound", buf->sound[i]);
	}

	getStringFromFile(fin, strbuf);
	if (strbuf != "end") {
		throw GameException(__LINE__, __func__, "Error: error level.path - end of unit");
	}

	return buf;
}

Union* GameManager::LoadUnion(ifstream& fin) {
	Union* buf = new Union();
	string strbuf;
	ReadFromFile(fin, "num", buf->num);
	if (buf->num <= 0) {
		throw GameException(__LINE__, __func__, "Error: error level.path - read num of union");
	}
	buf->place.resize(buf->num);
	buf->quat.resize(buf->num);
	for (int i = 0; i < buf->num; i++) {
		ReadFromFile(fin, "place", buf->place[i]);
		ReadFromFile(fin, "quat", buf->quat[i]);
	}
	bool flag = true;
	while (flag) {
		getStringFromFile(fin, strbuf);
		if (strbuf == "null" || strbuf == "end_of_file") {
			throw GameException(__LINE__, __func__, "Error: file level.file - union - wrong");
		}
		else if (strbuf == "unit") {
			Unit* unitbuf = LoadUnit(fin);
			buf->units.push_back(unitbuf);
		}
		else if (strbuf == "union") {
			Union* unionbuf = LoadUnion(fin);
			buf->unions.push_back(unionbuf);
		}
		else if (strbuf == "end") {
			flag = false;
			break;
		}
		else {
			throw GameException(__LINE__, __func__, "Error: level.file - wrong name");
		}
	}

	return buf;
}

void GameManager::LoadInfoAboutModels(uint levelNumber) {
	string strbuf = Levels.at(levelNumber).pathLoader_;
	std::ifstream fin(strbuf);
	if (!fin.is_open()) {
		throw GameException(__LINE__, __func__, "Error: file level.file - can't open");
	}
	LoadSound(fin);
	while (!fin.eof()) {
		getStringFromFile(fin, strbuf);
		if (strbuf == "null") {
			throw GameException(__LINE__, __func__, "Error: file level.file - null");
		}
		else if (strbuf == "end_of_file") {
			break;
		}
		else if (strbuf == "unit") {
			Unit* buf = LoadUnit(fin);
			MadeModels(buf);
			buf->~Unit();
		}
		else if (strbuf == "union") {
			Union* buf = LoadUnion(fin);
			MadeModels(buf);
			buf->~Union();
		}
		else {
			throw GameException(__LINE__, __func__, "Error: level.file - wrong name");
		}
	}
	if (!Models.size()) {
		throw GameException(__LINE__, __func__, "Error: LoadedModels is empty");
	}
	fin.close();
	LoadModels();
}

void GameManager::RenderModels(const mat4& projection, const mat4& view, const Camera& camera, float time) {
	Shader.Use();
	Light.SetLight(Shader);
	Shader.setMat4("projection", projection);
	Shader.setMat4("view", view);
	Shader.setVec3("viewPos", camera.Position);
	for (int i = 0; i<Models.size();i++) {
		if (i == 0) {
			//Models[i]->quat_.x = -camera.Pitch;
			Models[i]->quat_.y = -camera.Yaw;
		}
		if (Models[i]->draw_) {
			Models[i]->SetShaderParameters(Shader, time);
			Models[i]->Draw(Shader);
		}
	}

	/*for (auto& it : Models) {
		if (it->draw_) {
			it->SetShaderParameters(Shader, time);
			it->Draw(Shader);
		}
	}*/
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
		music->setIsPaused();
		music->setPlayPosition(0);
		return true;
	}
}

void GameManager::RenderWorld(const mat4& projection, const mat4& view, const Camera& camera, float time) {
	real_world_.do_step(time, real_world_);
	vector<phys_body*> to_create = get_creation();
	int a = to_create.size();
	while (a > 0) {
		this->GetCopy(2, to_create[a - 1]);
	}
	RenderModels(projection, view, camera, time);
	box.RenderBox(camera, projection);
	text.RenderText(SysText);
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
	LoadedSounds.clear();
	music->setIsPaused(false);
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
				SysText.clear();
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