#ifndef SOUND_H
#define SOUND_H

#include "irrKlang.h"
#include <cstdio>
#include <vector>
#include <string>
#include <map>

#define SOADLENGTH 900.0
#define MAXDISTANCE 100.0
#define BACKGROUNDVOLUME 0.5
#define BACKVOICE_DISTANCE 100.0

using std::string;
using std::vector;
using std::map;

enum CharacterSound {
	DEAD_Sound,
	WALK_Sound,
	ATTACK_Sound,
	JUMP_Sound,
	ALIVE_Sound,
	BACKBATTLE_Sound
};

enum StructureSound {
	PASSIVE_Sound,
	ACTIVE_Sound
};

enum WorldSound {
	WIND_Sound,
	SCREAM1_Sound,
	SCREAM2_Sound,
	LAUGH1_Sound,
	LAUGH2_Sound,
	BIRDS1_Sound,
	BIRDS2_Sound
};

enum TypeSound {
	HERO_Sound,
	NPC_Sound,
	BUILDING_Sound,
	WORLD_Sound
};

class SoundCharacter {
public:
	irrklang::vec3df Pos;
	vector<string> Path;
	vector<irrklang::ISoundSource*> ObjectSounds;
	irrklang::ISoundEngine* engine3d;
	irrklang::ISound* Walk;
	enum CharacterSound Status;
	bool IsNear = true;
	SoundCharacter(irrklang::vec3df& pos0, const vector<string>& path0, enum CharacterSound stat0,
		irrklang::ISoundEngine* engine3d_, map<string, irrklang::ISoundSource*>& GlobalMusicDataBase) : Pos(pos0), Path(path0),
		Status(stat0), engine3d(engine3d_) {
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
	void Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum CharacterSound action);
	void DoSound();
};

class SoundHero : public SoundCharacter {
public:
	irrklang::ik_f32 L0 = SOADLENGTH;
	irrklang::ik_f32 dt;
	irrklang::ISound* SOAD;
	SoundHero() = delete;
	SoundHero(irrklang::vec3df& pos0, const vector<string>& path0, enum CharacterSound stat0, irrklang::ISoundEngine* engine3d_, map<string, irrklang::ISoundSource*>& GlobalMusicDataBase)
		: SoundCharacter::SoundCharacter(pos0, path0, stat0, engine3d_, GlobalMusicDataBase) {
		SOAD = engine3d->play3D(ObjectSounds[BACKBATTLE_Sound], Pos, true, true, true);
		SOAD->setVolume(BACKGROUNDVOLUME);
	}
	void Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum CharacterSound action, irrklang::ik_f32& dt);
	void DoSound();
};

class SoundStructure {
public:
	irrklang::vec3df Pos;
	vector<string> Path;
	std::vector<irrklang::ISoundSource*> ObjectSounds;
	enum StructureSound Status;
	irrklang::ISoundEngine* engine3d;
	bool IsNear;

	SoundStructure(irrklang::vec3df& pos0, const vector<string>& path0, enum StructureSound action, irrklang::ISoundEngine* engine3d_, map<string, irrklang::ISoundSource*>& GlobalMusicDataBase)
		: Pos(pos0), Path(path0), Status(action), engine3d(engine3d_) {
		if (Path.size() != 0) {
			ObjectSounds.push_back(GlobalMusicDataBase[Path[0]]);
		}
	}
	~SoundStructure() {
		ObjectSounds.clear();
		Path.clear();
	}

	void Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum StructureSound action);
	void DoSound();
};

class SoundWorld {
public:
	irrklang::vec3df Pos;
	vector<string> Path;
	vector<irrklang::ISoundSource*> ObjectSounds;
	enum WorldSound Status;
	irrklang::ISound* SoundMenu;
	irrklang::ik_f32 L0 = BACKVOICE_DISTANCE;
	irrklang::ik_f32 dt;
	irrklang::ISoundEngine* engine3d;

	SoundWorld(irrklang::vec3df& pos0, const vector<string>& path0, enum WorldSound stat0,
		irrklang::ISoundEngine* engine3d_, map<string, irrklang::ISoundSource*>& GlobalMusicDataBase)
		: Pos(pos0), Path(path0), Status(stat0), engine3d(engine3d_) {
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

	void Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum WorldSound action, irrklang::ik_f32& dt0);
	void DoSound();
};

#endif