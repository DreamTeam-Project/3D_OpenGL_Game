#include "Manager.h"

GameManager::GameManager() : 
	Loading(LoadImage, true), Menu(MenuImage), text(FontFile), box(DarkStormy, lightSky), Shader("Light.vs", "Light.fs"),
	play(false), real_world_(phys_world(&mobs)), accum_shoot(0), flag_shoot(false), accum_fire(0), mobs(0)
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
		Light.SpotLights.push_back(SpotLight(NewModel, vec3(0.0f), vec3(1.0f, 1.0f, 0.8f), vec3(0.5f), camera.Front, vec3(0.0f), 1.0f, 0.014f, 0.0007f, 5.0f, 22.5f));
		Light.SpotLights.push_back(SpotLight(NewModel, vec3(0.0f), vec3(20.0f, 10.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), camera.Front, vec3(1.0f, 0.0f, 0.0f), 1.0f, 0.09f, 0.032f, 5.0f, 40.5f, false));
		break;
	case STRUCTURE:
		NewModel = new Structure(real_world_, type, place, quat, path, scale, mass, box[0], 8.0f, true);
		break;
	case BULLET:
		NewModel = new BulletModel(real_world_, type, place, quat, path, scale, mass, box[0], 32.0f, true);
		break;
	case BOX_BULLET:
		NewModel = new BulletBoxModel(real_world_, type, place, quat, path, scale, mass, box[0], 32.0f, true);
		Light.PointLights.push_back(PointLight(NewModel, vec3(0.0f, 5.0f, 0.0f), vec3(1.0f), vec3(0.2f), vec3(0.0f), 0.09f, 1.0f, 0.032f));
		break;
	case FLOOR:
		NewModel = new FloorModel(real_world_, type, place, quat, path, scale, mass, box[0], sounds, engine3d, LoadedSounds, 8.0f, true);
		break;
	case ENEMY_CLOSE:
		mobs++;
		NewModel = new EnemyCloseModel(real_world_, type, place, quat, path, scale, mass, box[0], sounds, engine3d, LoadedSounds, 32.0f, true);
		break;
	case ENEMY_DIS:
		mobs++;
		NewModel = new EnemyDisModel(real_world_, type, place, quat, path, scale, mass, box[0], sounds, engine3d, LoadedSounds, 32.0f, true);
		break;
	case HP_BOX:
		NewModel = new XPBoxModel(real_world_, type, place, quat, path, scale, mass, box[0], 32.0f, true);
		Light.PointLights.push_back(PointLight(NewModel, vec3(0.0f, 5.0f, 0.0f), vec3(1.0f), vec3(0.2f), vec3(0.0f), 0.09f, 1.0f, 0.032f));
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
	
	Models[0]->quat_.y = 90 - camera.Yaw;
	
	while (Models[0]->quat_.y >= 360) {
		Models[0]->quat_.y -= 360;
	}
	while (Models[0]->quat_.y <= -360) {
		Models[0]->quat_.y += 360;
	}
	//printf(".y = %f\n", Models[0]->quat_.y);
	/*if (Models[0]->quat_.y <= 90 || Models[0]->quat_.y >=260) {
		Models[0]->quat_.x = -camera.Pitch;
	}
	if (Models[0]->quat_.y >= 90 && Models[0]->quat_.y <= 260) {
		Models[0]->quat_.x = camera.Pitch;
	}*/
	/*Models[0]->quat_.z = camera.Pitch;
	Models[0]->quat_.x =90;*/
	//Models[0]->quat_.x =- camera.Pitch;
	while (Models[0]->quat_.x >= 360) {
		Models[0]->quat_.x -= 360;
	}
	while (Models[0]->quat_.x <= -360) {
		Models[0]->quat_.x += 360;
	}
	//printf(".x = %f\n", Models[0]->quat_.x);
	Shader.Use();
	Light.SetLight(Shader);
	Shader.setMat4("projection", projection);
	Shader.setMat4("view", view);
	Shader.setVec3("viewPos", camera.Position);
	for (int i = 0; i<Models.size();i++) {
		if (Models[i]->draw_) {
			Models[i]->SetShaderParameters(Shader, time);
			Models[i]->Draw(Shader);
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
		music->setIsPaused();
		music->setPlayPosition(0);
		return true;
	}
}

void GameManager::RenderWorld(const mat4& projection, const mat4& view, const Camera& camera, float time) {
	ProcessInputInGame(game_window, time);

	accum_shoot -= time;
	accum_fire -= time;
	if (accum_shoot <= 0) {
		accum_shoot = 0;
	}
	if (accum_fire <= 0) {
		accum_fire = 0;
		Light.SpotLights[1].light_on = false;
	}
	if (flag_shoot) {
		phys_body* tmp = get_camera()->aim(real_world_);
		if (tmp != nullptr) {
			GetCopy(1, tmp);
			Light.SpotLights[1].light_on = true;
		}
		
		accum_fire = 0.04;
		flag_shoot = false;
	}
	real_world_.do_step(time, real_world_);
	RenderModels(projection, view, camera, time);
	box.RenderBox(camera, projection);
	text.RenderText(string("o"), (float)HEIGHT / 500 * 248, (float)WIDTH / 9 * 4, 0.2f, vec3(1.0f, 0.0f, 0.0f));
	text.RenderText(string("Mobs ") + to_string(mobs), (float)HEIGHT / 50 * 1, (float)WIDTH / 50 * 43, 0.8f, vec3(1.0f, 0.0f, 0.0f));
	text.RenderText(string("HP ") + to_string(camera.position->getHealth()), (float)HEIGHT / 50 * 1, (float)WIDTH / 50 * 47, 0.8f, vec3(1.0f, 0.0f, 0.0f));
	text.RenderText(string("BT ") + to_string(camera.position->get_bullets()), (float)HEIGHT / 50 * 1, (float)WIDTH / 50 * 45, 0.8f, vec3(1.0f, 0.0f, 0.0f));
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

void GameManager::ProcessInputInEnd(GLFWwindow* window, uint& key_pressed) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		key_pressed = 3;
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		key_pressed = 1;
		return;
	}
}

void GameManager::EndLevel() {
	uint key = 0;
	ProcessInputInEnd(game_window, key);
	real_world_.clear_world();
	if (mobs == 0) {
		Menu.RenderImage(true);
	}
	bool status = false;
	while (!status && !glfwWindowShouldClose(game_window)) {
		if (mobs == 0) {
			text.RenderText(string("you saved the world"), (float)HEIGHT / 50 * 13, (float)WIDTH / 50 * 32, 0.8f, vec3(0.2f));
			text.RenderText(string("you are so cool"), (float)HEIGHT / 50 * 13, (float)WIDTH / 50 * 30, 0.8f, vec3(0.2f));
			text.RenderText(string("but its a battle"), (float)HEIGHT / 50 * 13, (float)WIDTH / 50 * 28, 0.8f, vec3(0.2f));
			text.RenderText(string("theres gonna be a war"), (float)HEIGHT / 50 * 13, (float)WIDTH / 50 * 26, 0.8f, vec3(0.2f));
		}
		if (camera.position->getHealth() <= 0) {
			text.RenderText(string("the GOAT warrior"), (float)HEIGHT / 50 * 16, (float)WIDTH / 50 * 32, 0.8f, vec3(1.0f, 0.0f, 0.0f));
			text.RenderText(string("the awesome guy"), (float)HEIGHT / 50 * 16, (float)WIDTH / 50 * 30, 0.8f, vec3(1.0f, 0.0f, 0.0f));
			text.RenderText(string("see you in glory "), (float)HEIGHT / 50 * 16, (float)WIDTH / 50 * 28, 0.8f, vec3(1.0f, 0.0f, 0.0f));
			text.RenderText(string(" of Odins hall "), (float)HEIGHT / 50 * 16, (float)WIDTH / 50 * 26, 0.8f, vec3(1.0f, 0.0f, 0.0f));
		}
		if (key == 1) {
			text.RenderText(string("you left the battle"), (float)HEIGHT / 50 * 13, (float)WIDTH / 50 * 32, 0.8f, vec3(1.0f, 0.0f, 0.0f));
			text.RenderText(string("and you will never see"), (float)HEIGHT / 50 * 13, (float)WIDTH / 50 * 30, 0.8f, vec3(1.0f, 0.0f, 0.0f));
			text.RenderText(string("the greatest ever view"), (float)HEIGHT / 50 * 13, (float)WIDTH / 50 * 28, 0.8f, vec3(1.0f, 0.0f, 0.0f));
			text.RenderText(string("from top of Odins hall"), (float)HEIGHT / 50 * 13, (float)WIDTH / 50 * 26, 0.8f, vec3(1.0f, 0.0f, 0.0f));
		}
		text.RenderText(string("Press H to go to menu"), (float)HEIGHT / 25 * 7.7, (float)WIDTH / 50 * 40, 0.7f, vec3(0.5f));
		ProcessInputInEnd(game_window, key);
		if (key == 3) {
			status = true;
		}
		glfwSwapBuffers(game_window);
		glfwPollEvents();
	}
	for (auto& it : LoadedSounds) {
		engine3d->removeSoundSource(it.first.c_str());
	}
	music->setIsPaused(false);
	Light.Clear();
	accum_shoot = 0;
	accum_fire = 0;
	mobs = 0;
	SysText.clear();
	Models.clear();
	LoadedModels.clear();
	LoadedSounds.clear();
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
	
	SysText.clear();

	SysText.push_back(SysStrings(GameName, (float)HEIGHT / 4.5f, (float)WIDTH / 7 * 6, 0.85f, vec3(0.2f)));
	for (uint i = 0; i < Levels.size(); i++) {
		SysText.push_back(SysStrings(Levels[i].name_, (float)HEIGHT / 6, (float)WIDTH / 14 * (11 - (i + 1)), 0.7f, vec3(0.4f)));
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

void GameManager::ProcessInputInGame(GLFWwindow *window, float deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (accum_shoot == 0) {
			flag_shoot = true;
			accum_shoot = 0.5;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		play = false;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.position->jump();
	}
}