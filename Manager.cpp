#include "Manager.h"

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
		levels.push_back(Level(name_buf, path_buf));
	}
	if (levels.levels_.size() == 0) {
		throw GameException(__LINE__, __func__, "LoadFile.file is empty");
	}
#if DEBUG_MANAGER
	print("success\n");
#endif
}

void GameManager::LoadInfoAboutModels(size_t levelNumber) {
	string path = levels.levels_.at(levelNumber).pathLoader_;
	std::ifstream fin(path);
	if (!fin.is_open()) {
		throw GameException(__LINE__, __func__, "error open file level.file");
	}

#if DEBUG_MANAGER
	print(string("try to read: ") + path);
#endif
	int type;
	vec3 place;
	vec3 quat;
	string path_2;
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
		getStringFromFile(fin, path_2);

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
				NewModel = new GameModel(real_world_, type, place, quat, path_2, scale, mass, box);
				break;
			case ANIMATION:
				NewModel = new AnimatedModel(real_world_, type, place, quat, path_2, scale, mass, box);
				break;
			case STRUCTURE:
				NewModel = new Structure(real_world_, type, place, quat, path_2, scale, mass, box);
				break;
			default:
				NewModel = new GameModel(real_world_, type, place, quat, path_2, scale, mass, box);
			}
		}
		else {
			throw GameException(__LINE__, __func__, "Error wrong type of class");
	}
		//real_world_ ,
		//NewModel->rigid_body_();
		//
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
	for (auto it : AllModels) {
		it->LoadModel();
	}
}


GameManager::GameManager() : real_world_(phys_world()) {}

//GameManager::~GameManager() {
//	real_world_.~phys_world();
//}