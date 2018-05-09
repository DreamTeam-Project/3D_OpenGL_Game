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
				NewModel = new Structure(32.0f, true);
				break;
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
}