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

#ifndef GAMELEVEL3MODULE_H_
#define GAMELEVEL3MODULE_H_

#include "Module.h"
#include "Burst.h"
#include "Explosion.h"
#include "Shooter.h"
#include "GestureTrail.h"
#include "Gestures.h"
#include "Game.h"

struct Tomato
{
	float x,y;
	float xv,yv;
	float rot;
	int on;
};

class GameLevel3Module:public Module
{
private:

	Boss boss;

	FMOD::Sound *bgm;
	FMOD::Channel *bgmchannel;

	FMOD::Sound *bgmintro;
	FMOD::Channel *bgmintrochannel;

	Wizard wiz;
	int posX,posY;

	float pillar1Y,pillar2Y;

	int stage;

	Shooter shot;

	int partEngIDs[3];

	ParticleEngineChild* burst;
	ParticleEngineChild* explosion;

	unsigned long shieldTimer;
	float shieldPulse;

	Tomato tom;


// Level Timing
	unsigned long levelBeginTick;
	unsigned long menuLastTick;

// Character Animations
	int punching;
	unsigned long punchingTimer;

// Gesture menu
	int showMenu;

// IntroTicker
	unsigned long waitTick;

// Gestures
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
	unsigned long diedTick;

	int gestureTrailID;
	ParticleEngineChild* gestureTrail;

	unsigned long menuOpenedTick;
	int menuOpenedTickDiff;

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

extern Module* GameLevel3Mod;

#endif