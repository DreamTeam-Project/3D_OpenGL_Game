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
		if (Status == DEAD_Sound && !IsDead) {
			Walk->setIsPaused();
			bool check = engine3d->isCurrentlyPlaying(ObjectSounds[DEAD_Sound]);
			if (!check) {
				engine3d->play3D(ObjectSounds[DEAD_Sound], Pos);
			}
			IsDead = true;
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
			Walk->setIsPaused();
			bool check = engine3d->isCurrentlyPlaying(ObjectSounds[ALIVE_Sound]);
			if (!check) {
				engine3d->play3D(ObjectSounds[ALIVE_Sound], Pos);
			}
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
		if (L0 > boom->getPlayLength - 10) {
			boom->setIsPaused();
			boom->setPlayPosition(0);
		}
		if (L0 < SOADLENGTH) {
			L0 = L0 + dt;
		}
		if (L0 > SOADLENGTH) {
			SOAD->setIsPaused();
			SOAD->setPlayPosition(0);
		}
	}
	if (Status == DEAD_Sound && !IsDead) {
		Walk->setIsPaused();
		bool check = engine3d->isCurrentlyPlaying(ObjectSounds[DEAD_Sound]);
		if (!check) {
			engine3d->play3D(ObjectSounds[DEAD_Sound], Pos);
		}
		IsDead = true;
	}
	if (Status == ATTACK_Sound) {
		L0 = L0 + dt;
		Walk->setIsPaused();
		bool check = engine3d->isCurrentlyPlaying(ObjectSounds[ATTACK_Sound]);
		if (!check) {
			boom->setIsPaused(false);
		}
		else {
			boom->setPlayPosition(0);
			boom->setIsPaused(false);
		}
		bool check2 = engine3d->isCurrentlyPlaying(ObjectSounds[BACKBATTLE_Sound]);
		if (check2) {
			SOAD->setIsPaused(false);
		}
		L0 = 0;
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
		Walk->setIsPaused();
		bool check = engine3d->isCurrentlyPlaying(ObjectSounds[ALIVE_Sound]);
		if (!check) {
			engine3d->play3D(ObjectSounds[ALIVE_Sound], Pos);
		}
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
	if (Path.size() != 0) {
		this->DoSound();
	}
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
	if (L0 < 0) {
		int TypeOfSound = rand() % 7;
		if (TypeOfSound == 1) {
			Status = WIND_Sound;
		}
		if (TypeOfSound == 2) {
			Status = SCREAM1_Sound;
		}
		if (TypeOfSound == 3) {
			Status = SCREAM2_Sound;
		}
		if (TypeOfSound == 4) {
			Status = LAUGH1_Sound;
		}
		if (TypeOfSound == 5) {
			Status = LAUGH2_Sound;
		}
		if (TypeOfSound == 6) {
			Status = BIRDS1_Sound;
		}
		if (TypeOfSound == 0) {
			Status = BIRDS2_Sound;
		}
		L0 = BACKVOICE_DISTANCE;
	}

	this->DoSound();
}

void SoundWorld::DoSound() {

	if (Status == WIND_Sound) {
		engine3d->play3D(ObjectSounds[WIND_Sound], Pos);
	}
	if (Status == SCREAM1_Sound) {
		engine3d->play3D(ObjectSounds[SCREAM1_Sound], Pos);
	}
	if (Status == LAUGH1_Sound) {
		engine3d->play3D(ObjectSounds[LAUGH1_Sound], Pos);
	}
	if (Status == BIRDS1_Sound) {
		engine3d->play3D(ObjectSounds[BIRDS1_Sound], Pos);
	}
	if (Status == SCREAM2_Sound) {
		engine3d->play3D(ObjectSounds[SCREAM2_Sound], Pos);
	}
	if (Status == LAUGH2_Sound) {
		engine3d->play3D(ObjectSounds[LAUGH2_Sound], Pos);
	}
	if (Status == BIRDS2_Sound) {
		engine3d->play3D(ObjectSounds[BIRDS2_Sound], Pos);
	}
