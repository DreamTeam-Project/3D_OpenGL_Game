#include "Sound.h"

void SoundCharacter::Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum CharacterSound action) {
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
		if (Status == DEAD_Sound) {
			Walk->setIsPaused();
			bool check = engine3d->isCurrentlyPlaying(ObjectSounds[DEAD_Sound]);
			if (!check) {
				engine3d->play3D(ObjectSounds[DEAD_Sound], Pos);
			}
		}
		if (Status == ATTACK_Sound) {
			Walk->setIsPaused();
			bool check = engine3d->isCurrentlyPlaying(ObjectSounds[ATTACK_Sound]);
			if (!check) {
				engine3d->play3D(ObjectSounds[ATTACK_Sound], Pos);
			}
		}
		if (Status == JUMP_Sound) {
			Walk->setIsPaused();
			bool check = engine3d->isCurrentlyPlaying(ObjectSounds[JUMP_Sound]);
			if (!check) {
				engine3d->play3D(ObjectSounds[JUMP_Sound], Pos);
			}
		}
		if (Status == WALK_Sound) {
			Walk->setIsPaused(false);
		}
		if (Status == ALIVE_Sound) {
			//live as long as you can
		}
	}
	else {
		Walk->setIsPaused();
	}

}

void SoundHero::Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum CharacterSound action, irrklang::ik_f32& dt0) {
	Pos = pos0;
	dt = dt0;
	Status = action;
	Walk->setPosition(Pos);
	this->DoSound();
}

void SoundHero::DoSound() {
		if (Status != ATTACK_Sound) {
			if (L0 > 0) {
				L0 = L0 - dt;
			}
			if (L0 <= 0) {
				SOAD->setIsPaused();
				SOAD->setPlayPosition(0);
			}
		}
		if (Status == DEAD_Sound) {
			Walk->setIsPaused();
			bool check = engine3d->isCurrentlyPlaying(ObjectSounds[DEAD_Sound]);
			if (!check) {
				engine3d->play3D(ObjectSounds[DEAD_Sound], Pos);
			}

		}
		if (Status == ATTACK_Sound) {
			L0 = SOADLENGTH;
			Walk->setIsPaused();
			bool check = engine3d->isCurrentlyPlaying(ObjectSounds[ATTACK_Sound]);
			if (!check) {
				engine3d->play3D(ObjectSounds[ATTACK_Sound], Pos);
			}
			bool check2 = engine3d->isCurrentlyPlaying(ObjectSounds[BACKBATTLE_Sound]);
			if (check2) {
				SOAD->setIsPaused(false);
			}
		}
		if (Status == JUMP_Sound) {
			Walk->setIsPaused();
			bool check = engine3d->isCurrentlyPlaying(ObjectSounds[JUMP_Sound]);
			if (!check) {
				engine3d->play3D(ObjectSounds[JUMP_Sound], Pos);
			}
		}
		if (Status == WALK_Sound) {
			Walk->setIsPaused(false);
		}
		if (Status == ALIVE_Sound) {
			//live, die, to sides of one pie....
		}

}

void SoundStructure::Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum StructureSound action) {
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
	if (IsNear && (Status == ACTIVE_Sound)) {
		bool check = engine3d->isCurrentlyPlaying(ObjectSounds[0]);
		if (!check) {
			engine3d->play3D(ObjectSounds[0], Pos);
		}
	}
}

void SoundWorld::Refresh(irrklang::vec3df& pos0, irrklang::vec3df& lpos0, enum WorldSound action, irrklang::ik_f32& dt0) {
	Pos = pos0;
	dt = dt0;
	Status = action;
	irrklang::vec3df dist = pos0 - lpos0;
	SoundMenu->setPosition(Pos);
	L0 = L0 - dt;
	if (!L0) {
		int TypeOfSound = rand() % 5;
		if (TypeOfSound == 1) {
			Status = WIND_Sound;
		}
		if (TypeOfSound == 2) {
			Status = SCREAM_Sound;
		}
		if (TypeOfSound == 3) {
			Status = LAUGH_Sound;
		}
		if (TypeOfSound == 4) {
			Status = BIRDS_Sound;
		}
		L0 = BACKVOICE_DISTANCE;
	}

	this->DoSound();
}

void SoundWorld::DoSound() {

	if (Status == MENU_Sound) {
		bool check = engine3d->isCurrentlyPlaying(ObjectSounds[MENU_Sound]);
		if (!check) {
			SoundMenu->setIsPaused(false);
		}
	}
	if (Status == WIND_Sound) {
		engine3d->play3D(ObjectSounds[WIND_Sound], Pos);
	}
	if (Status == SCREAM_Sound) {
		engine3d->play3D(ObjectSounds[SCREAM_Sound], Pos);
	}
	if (Status == LAUGH_Sound) {
		engine3d->play3D(ObjectSounds[LAUGH_Sound], Pos);
	}
	if (Status == BIRDS_Sound) {
		engine3d->play3D(ObjectSounds[BIRDS_Sound], Pos);
	}
	if (Status == GAME_Sound) {
		//find every easter egg
	}
}