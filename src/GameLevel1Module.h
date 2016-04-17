/*
 Copyright 2006, 2016 Jeff Verkoeyen. All Rights Reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#ifndef GAMELEVEL1MODULE_H_
#define GAMELEVEL1MODULE_H_

#include "Module.h"
#include "Burst.h"
#include "Explosion.h"
#include "Shooter.h"
#include "GestureTrail.h"
#include "Gestures.h"
#include "Zombie.h"
#include "Rain.h"
#include "Game.h"

#define NUMZOMBIES		(40)

class GameLevel1Module:public Module
{
private:
	int numZombiesDiff;
	FMOD::Sound *bgm;
	FMOD::Channel *bgmchannel;

	FMOD::Sound *bgmintro;
	FMOD::Channel *bgmintrochannel;

	Wizard wiz;
	int posX,posY;

	float levelOffset;
	Vector levelOffsetVect;

	int stage;

	Shooter shot;

	int partEngIDs[3];

	ParticleEngineChild* burst;
	ParticleEngineChild* explosion;
	ParticleEngineChild* rain;

	float mountainOffset;

	unsigned long bossTick;
	int bossAnimation;

// INTRO
	Zombie introZombies[4];

// Level Timing
	unsigned long levelBeginTick;
	unsigned long menuLastTick;

// Score Rendering
	scoreText *head,*curr;
	int combo;

// Character Animations
	int punching;
	unsigned long punchingTimer;

// Enemies
	Zombie zombies[NUMZOMBIES];

// Gesture menu
	int showMenu;


// Gestures
	unsigned long rainSlowTick;
	int doGesture;

	int index;

	Directions* spell;
	Directions clickedDirection;

	int showingSpell;
	unsigned long showSpellTimer;
	unsigned long spellTime;

	Vector clickStart;
	Vector lastPos;

	unsigned long lastTick;
	unsigned long gestureTick;

	int gestureTrailID;
	ParticleEngineChild* gestureTrail;

	void renderGestures();

	void input();
	void coldet();
	void render();
	void render2();
	void update();
public:
	void Initialize();
	Module* ExecuteModule();
	void Shutdown();
	void GiveData(void*);
};

extern Module* GameLevel1Mod;

#endif