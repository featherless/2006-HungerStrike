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

#ifndef LEVEL1MODULE_H_
#define LEVEL1MODULE_H_

#include "Module.h"
#include "cVector.h"
#include "GestureTrail.h"
#include "Gestures.h"
#include "Tower.h"
#include "Burst.h"
#include "Explosion.h"
#include "Shooter.h"

class Level1Module:public Module
{
private:
	int index;

	Tower tower1;
	Tower tower2;

	int numSpells;
	Directions* spell;
	Directions clickedDirection;

	Vector clickStart;
	Vector lastPos;

	unsigned long lastTick;

	int posX,posY;

	int gestureTrailID;
	ParticleEngineChild* gestureTrail;
public:
	void Initialize();
	Module* ExecuteModule();
	void Shutdown();
	void GiveData(void*);
};

extern Module* Level1Mod;

#endif