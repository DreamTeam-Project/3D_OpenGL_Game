#include "irrKlang.h"

#define SOADLENGTH 900.0
#define MAXDISTANCE 100.0
#define BACKGROUNDVOLUME 0.5
#define BACKVOICE_DISTANCE 100.0

#pragma comment(lib, "irrKlang.lib")


enum Character {
	DEAD,
	WALK,
	ATTACK,
	JUMP,
	ALIVE,
	BACKBATTLE
};

enum Structure {
	PASSIVE,
	ACTIVE
};

enum World {
	MENU,
	WIND,
	SCREAM,
	LAUGH,
	BIRDS,
	GAME
};

enum Type {
	HERO,
	NPC,
	BUILDING,
	WORLD
};


class SoundCharacter {
public:
	irrklang::vec3df Pos;
	//irrklang::vec3df ListnerPos;
	//irrklang::vec3df SoundPos;
	std::vector<std::string> Path;
	std::vector<irrklang::ISoundSource*> ObjectSounds;
	irrklang::ISound* Walk;
	enum Character Status;
	bool IsNear = true;
	SoundCharacter(irrklang::vec3df& pos0, std::vector<std::string>& path0, enum Character stat0) : Pos(pos0), Path(path0), Status(stat0) {
		for (int i = 0; i < Path.size(); i++) {
			ObjectSounds.push_back(GlobalMusicDataBase[Path[i]]);
		}
		Walk = engine3d->play3D(ObjectSounds[1], Pos, true, true, true);
	}
	~SoundCharacter() {
		Path.clear();
		ObjectSounds.clear();
		Walk->drop();
	}
	void Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum Character action);
	void DoSound();

};


class SoundHero : public SoundCharacter {
public:
	irrklang::ik_f32 L0 = SOADLENGTH;
	irrklang::ik_f32 dt;
	irrklang::ISound* SOAD;
	SoundHero() = delete;
	SoundHero(irrklang::vec3df& pos0, std::vector<std::string>& path0, enum Character stat0) : SoundCharacter::SoundCharacter(pos0, path0, stat0) {
		SOAD = engine3d->play3D(ObjectSounds[BACKBATTLE], Pos, true, true, true);
		SOAD->setVolume(BACKGROUNDVOLUME);
	}
	void Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum Character action, irrklang::ik_f32& dt);
	void DoSound();
};


class SoundStructure {
	irrklang::vec3df Pos;
	std::vector<std::string> Path;
	std::vector<irrklang::ISoundSource*> ObjectSounds;
	enum Structure Status;
	bool IsNear;

	SoundStructure(irrklang::vec3df& pos0, std::vector<std::string>& path0, enum Structure action) : Pos(pos0), Path(path0), Status(action) {
		ObjectSounds.push_back(GlobalMusicDataBase[Path[0]]);
	}
	~SoundStructure() {
		ObjectSounds.clear();
		Path.clear();
	}

	void Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum Structure action);
	void DoSound();
};


class SoundWorld {
	irrklang::vec3df Pos;
	std::vector<std::string> Path;
	std::vector<irrklang::ISoundSource*> ObjectSounds;
	enum World Status;
	irrklang::ISound* SoundMenu;
	//bool IsNear;
	irrklang::ik_f32 L0 = BACKVOICE_DISTANCE;
	irrklang::ik_f32 dt;

	SoundWorld(irrklang::vec3df& pos0, std::vector<std::string>& path0, enum World stat0) : Pos(pos0), Path(path0), Status(stat0) {
		for (int i = 0; i < Path.size(); i++) {
			ObjectSounds.push_back(GlobalMusicDataBase[Path[i]]);
		}
		SoundMenu = engine3d->play3D(ObjectSounds[0], Pos, true, true);
	}

	~SoundWorld() {
		ObjectSounds.clear();
		Path.clear();
		SoundMenu->drop();
	}

	void Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum World action, irrklang::ik_f32& dt0);
	void DoSound();

};


