
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <map>
#include "Sound.h"

/*
#define STREAM_SOUND "voices/stream1.mp3"
#define SPIDER_SOUND "voices/klac.mp3"
#define FLOOR "voices/floor.mp3"
#define SHOT "voices/drobovik.mp3"
#define CREEPY1 "voices/kacheli.mp3"
#define DESTINY "voices/destiny.mp3"
#define WALK_S "voices/shagi.mp3"
#define DOOR "voices/petla.mp3"
*/


using std::string;
using std::vector;

irrklang::ISoundEngine* engine3d = irrklang::createIrrKlangDevice();
//ADD TO DO_STEP
//engine3d->setListenerPosition(ListnerPos, LLook);

//map/vector/aray of sounds;


std::map<std::string, irrklang::ISoundSource*> GlobalMusicDataBase;

struct OuterSoundStaff {
	std::vector<std::string> Path;
	//enum Type kind;
	//MAYBE SOMTH ELSE

};

struct InnerSoundStaff {
	std::vector<irrklang::ISound*> ObjectSounds;
};

void SoundCharacter::Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum Character action) {
	Pos = pos0;
	Status = action;
	irrklang::vec3df dist = pos0 - lpos0;
	Walk->setPosition(Pos);
	if (dist.getLength() < (irrklang::ik_f32)MAXDISTANCE) {
		IsNear = true;
	}
	else {
		IsNear = false;
	}
	this->DoSound();
}

void SoundCharacter::DoSound() {
	if (IsNear) {
		if (Status == DEAD) {
			Walk->setIsPaused();
			bool check = engine3d->isCurrentlyPlaying(ObjectSounds[DEAD]);
			if (!check) {
				engine3d->play3D(ObjectSounds[DEAD], Pos);
			}
		}
		if (Status == ATTACK) {
			Walk->setIsPaused();
			bool check = engine3d->isCurrentlyPlaying(ObjectSounds[ATTACK]);
			if (!check) {
				engine3d->play3D(ObjectSounds[ATTACK], Pos);
			}
		}
		if (Status == JUMP) {
			Walk->setIsPaused();
			bool check = engine3d->isCurrentlyPlaying(ObjectSounds[JUMP]);
			if (!check) {
				engine3d->play3D(ObjectSounds[JUMP], Pos);
			}
		}
		if (Status == WALK) {
			Walk->setIsPaused(false);
		}
		if (Status == ALIVE) {
			//live as long as you can
		}
	}
	else {
		Walk->setIsPaused();
	}

}

void SoundHero::Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum Character action, irrklang::ik_f32& dt0) {
	Pos = pos0;
	dt = dt0;
	Status = action;
	//irrklang::vec3df dist = pos0 - lpos0;
	Walk->setPosition(Pos);
	/*if (dist.getLength() < (irrklang::ik_f32)MAXDISTANCE) {
		IsNear = true;
	}
	else {
		IsNear = false;
	}*/
	this->DoSound();
}

void SoundHero::DoSound() {
		if (Status != ATTACK) {
			if (L0 > 0) {
				L0 = L0 - dt;
			}
			if (L0 <= 0) {
				SOAD->setIsPaused();
				SOAD->setPlayPosition(0);
			}
		}
		if (Status == DEAD) {
			Walk->setIsPaused();
			bool check = engine3d->isCurrentlyPlaying(ObjectSounds[DEAD]);
			if (!check) {
				engine3d->play3D(ObjectSounds[DEAD], Pos);
			}

		}
		if (Status == ATTACK) {
			L0 = SOADLENGTH;
			Walk->setIsPaused();
			bool check = engine3d->isCurrentlyPlaying(ObjectSounds[ATTACK]);
			if (!check) {
				engine3d->play3D(ObjectSounds[ATTACK], Pos);
			}
			bool check2 = engine3d->isCurrentlyPlaying(ObjectSounds[BACKBATTLE]);
			if (check2) {
				SOAD->setIsPaused(false);
			}
		}
		if (Status == JUMP) {
			Walk->setIsPaused();
			bool check = engine3d->isCurrentlyPlaying(ObjectSounds[JUMP]);
			if (!check) {
				engine3d->play3D(ObjectSounds[JUMP], Pos);
			}
		}
		if (Status == WALK) {
			Walk->setIsPaused(false);
		}
		if (Status == ALIVE) {
			//live, die, to sides of one pie....
		}

}

void SoundStructure::Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum Structure action) {
	Pos = pos0;
	Status = action;
	irrklang::vec3df dist = pos0 - lpos0;
	if (dist.getLength() < (irrklang::ik_f32)MAXDISTANCE) {
		IsNear = true;
	}
	else {
		IsNear = false;
	}
	this->DoSound();
}

void SoundStructure::DoSound() {
	if (IsNear && (Status == ACTIVE)) {
		bool check = engine3d->isCurrentlyPlaying(ObjectSounds[0]);
		if (!check) {
			engine3d->play3D(ObjectSounds[0], Pos);
		}
	}
}

void SoundWorld::Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum World action, irrklang::ik_f32& dt0) {
	Pos = pos0;
	dt = dt0;
	Status = action;
	irrklang::vec3df dist = pos0 - lpos0;
	SoundMenu->setPosition(Pos);
	L0 = L0 - dt;
	if (!L0) {
		int TypeOfSound = rand() % 5;
		if (TypeOfSound == 1) {
			Status = WIND;
		}
		if (TypeOfSound == 2) {
			Status = SCREAM;
		}
		if (TypeOfSound == 3) {
			Status = LAUGH;
		}
		if (TypeOfSound == 4) {
			Status = BIRDS;
		}
		L0 = BACKVOICE_DISTANCE;
	}

	this->DoSound();
}

void SoundWorld::DoSound() {

	if (Status == MENU) {
		bool check = engine3d->isCurrentlyPlaying(ObjectSounds[MENU]);
		if (!check) {
			SoundMenu->setIsPaused(false);
		}
	}
	if (Status == WIND) {
		engine3d->play3D(ObjectSounds[WIND], Pos);
	}
	if (Status == SCREAM) {
		engine3d->play3D(ObjectSounds[SCREAM], Pos);
	}
	if (Status == LAUGH) {
		engine3d->play3D(ObjectSounds[LAUGH], Pos);
	}
	if (Status == BIRDS) {
		engine3d->play3D(ObjectSounds[BIRDS], Pos);
	}
	if (Status == GAME) {
		//find every easter egg
	}
}

/*
class SoundObject {
private:
//const char* Name[4];
//bool IsLooped[4] = { false , false, false , false };
irrklang::E_STREAM_MODE IsStreamed = irrklang::ESM_NO_STREAMING;
bool IsNear = false;
enum Status Action;
irrklang::ik_f32 Volume = 1.0;
public:

//bool flag[4] = { true, false, false, false };
//void StartStop();
//irrklang::vec3df pos;
//irrklang::vec3df ListnerPos;
//irrklang::ik_f32 GetVolume();
//void SetVolume(irrklang::ik_f32 setv);
//bool GetIsStreamed();
//void Refresh(irrklang::vec3df pos0, irrklang::vec3df Lpos, int action);

//int GetIsAction();
//int GetIsNear();
//int GetIsLooped();
//const char* GetName(int n);
//irrklang::ISound* music[4];
//void SetAction(enum action);
//void SetIsNear(bool Near = false);
//void SetIsLooped(int n, bool Looped = false);
//void SetName(const char* NewName);
//void SetIsStreamed(bool streamed);
SoundObject(irrklang::vec3df pos0, irrklang::vec3df Lpos, bool Stream, const char* FirstMusic, const char* SecondMusic = "\0", const char* ThirdMusic = "\0", const char* FourthMusic = "\0") {
pos = pos0;
ListnerPos = Lpos;
pos = pos - ListnerPos;
if (Stream) {
IsStreamed = irrklang::ESM_STREAMING;
}
else {
IsStreamed = irrklang::ESM_NO_STREAMING;
}

Name[0] = FirstMusic;
music[0] = engine3d->play3D(Name[0], pos, true, true, true, IsStreamed, false);
music[0]->getPlayPosition();
if (SecondMusic != "\0") {
Name[1] = SecondMusic;
//music[1] = engine3d->play3D(Name[1], pos, false, true, true, IsStreamed, false);
flag[1] = true;
}
if (ThirdMusic != "\0") {
Name[2] = SecondMusic;
//music[2] = engine3d->play3D(Name[2], pos, false, true, true, IsStreamed, false);
flag[2] = true;
}
if (FourthMusic != "\0") {
Name[3] = SecondMusic;
//music[3] = engine3d->play3D(Name[3], pos, false, true, true, IsStreamed, false);
flag[3] = true;
}

//music->setPosition()
}
~SoundObject() {
music[0]->drop();
free(Name);
free(IsLooped);
free(flag);
free(music);
}
};
*/

/*
irrklang::ik_f32 SoundObject::GetVolume() {
	return Volume;
}

void SoundObject::SetVolume(irrklang::ik_f32 setv) {
	Volume = setv;
}

void SoundObject::SetIsStreamed(bool streamed) {
	if (streamed == true) {
		IsStreamed = irrklang::ESM_STREAMING;
	}
	else {
		IsStreamed = irrklang::ESM_NO_STREAMING;
	}
}

bool SoundObject::GetIsStreamed() {
	if (IsStreamed == irrklang::ESM_NO_STREAMING) {
		return 0;
	}
	if (IsStreamed == irrklang::ESM_STREAMING) {
		return 1;
	}
}


void SoundObject::StartStop() {
	if (IsNear) {
		if (Action == WALK) {
			music[0]->setIsPaused(false);
		}
		if (Action == SHOOT ) {
			music[0]->setIsPaused();
			if(flag[1])
				engine3d->play3D(Name[1], pos, false, false, true, IsStreamed, false);
		}
		if (Action == JUMP) {
			music[0]->setIsPaused();
			if (flag[2])
				engine3d->play3D(Name[2], pos, false, false, true, IsStreamed, false);
		}
		if (Action == ALIVE) {
			music[0]->setIsPaused();
		}
		if (Action == DEAD) {
			music[0]->getIsPaused();
			if (flag[3])
				engine3d->play3D(Name[3], pos, false, false, true, IsStreamed, false);
		}
	}
	else {
		music[0]->setIsPaused();
	}

}


int SoundObject::GetIsAction() {
	if (IsAction) {
		return 1;
	}
	else {
		return 0;
	}
}*/

/*int SoundObject::GetIsNear() {
	if (IsNear) {
		return 1;
	}
	else {
		return 0;
	}
}*/

/*
int SoundObject::GetIsLooped() {
	if (IsLooped) {
		return 1;
	}
	else {
		return 0;
	}
}

const char* SoundObject::GetName(int n) {
	return Name[n];
}

void SoundObject::SetAction(enum action) { //HOW TO CHANGE STATUS???
	//Action = action;
	StartStop();
}

void SoundObject::SetIsNear(bool Near = false) {
	IsNear = Near;
	
	StartStop();
}*/

/*
void SoundObject::SetIsLooped(int n,bool Looped) {
	IsLooped[n] = Looped;
}
*/

/*void SoundObject::SetName(const char* NewName) {
	Name = NewName;
}*/

/*
int main(int argc, const char** argv)
{
	irrklang::ISoundSource* s0 = engine3d->addSoundSourceFromFile("voices/destiny.mp3", irrklang::ESM_STREAMING, true);
	GlobalMusicDataBase.insert(std::make_pair("voices/destiny.mp3", s0));
	irrklang::ISoundSource* s1 = engine3d->addSoundSourceFromFile("voices/RT.mp3", irrklang::ESM_STREAMING, true);
	GlobalMusicDataBase.insert(std::make_pair("voices/RT.mp3", s1));

	irrklang::ISoundSource* s2 = engine3d->addSoundSourceFromFile("voices/drobovik.mp3", irrklang::ESM_NO_STREAMING, true);
	GlobalMusicDataBase.insert(std::make_pair("voices/drobovik.mp3", s2));
	irrklang::ISoundSource* s3 = engine3d->addSoundSourceFromFile("voices/shagi.mp3", irrklang::ESM_STREAMING, true);
	GlobalMusicDataBase.insert(std::make_pair("voices/shagi.mp3", s3));
	irrklang::ISoundSource* s4 = engine3d->addSoundSourceFromFile("voices/klac.mp3", irrklang::ESM_NO_STREAMING, true);
	GlobalMusicDataBase.insert(std::make_pair("voices/klac.mp3", s4));
	irrklang::ISoundSource* s5 = engine3d->addSoundSourceFromFile("voices/SOAD.mp3", irrklang::ESM_STREAMING, true);
	GlobalMusicDataBase.insert(std::make_pair("voices/SOAD.mp3", s5));
	irrklang::vec3df pos0(1, 2, 4.5);
	irrklang::vec3df lpos(0, 0, 0);
	vector<string> path;
	path.push_back("voices/destiny.mp3");
	path.push_back("voices/RT.mp3");
	path.push_back("voices/drobovik.mp3");
	path.push_back("voices/shagi.mp3");
	path.push_back("voices/klac.mp3");
	path.push_back("voices/SOAD.mp3");
	irrklang::vec3df pos1(-1, -2, -5);
	enum Character type;
	type = ALIVE;
	irrklang::ik_f32 dt = 10.0;
	SoundHero tmp(pos0, path, type);
	tmp.Refresh( pos1, lpos, type, dt);
	printf("    %f\n", tmp.L0);
	system("pause");
	type = WALK;
	dt = 5.0;
	tmp.Refresh(pos0, lpos, type, dt);
	printf("    %f\n", tmp.L0);
	system("pause");
	type = ALIVE;
	tmp.Refresh(pos1, lpos, type, dt);
	printf("    %f\n", tmp.L0);
	system("pause");
	type = JUMP;
	tmp.Refresh(pos0, lpos, type, dt);
	printf("    %f\n", tmp.L0);
	system("pause");
	type = ATTACK;
	tmp.Refresh(pos1, lpos, type, dt);
	printf("    %f\n", tmp.L0);
	system("pause");
	type = ATTACK;
	tmp.Refresh(pos0, lpos, type, dt);
	printf("    %f\n", tmp.L0);
	type = WALK;
	dt = 0.01;

	while (1) {
		tmp.Refresh(pos1, lpos, type, dt);
		printf("   %f\n", tmp.L0);
	}
	engine3d->drop();

	return 0;
}
*/
