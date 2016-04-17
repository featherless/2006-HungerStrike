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

#ifndef GESTURESMODULE_H_
#define GESTURESMODULE_H_

#include "Module.h"
#include "cVector.h"
#include "GestureTrail.h"
#include "Gestures.h"

class GesturesModule:public Module
{
private:
	int index;

	Directions* spell;
	Directions clickedDirection;

	int showingSpell;
	unsigned long showSpellTimer;

	Vector clickStart;
	Vector lastPos;

	unsigned long lastTick;

	int gestureTrailID;
	ParticleEngineChild* gestureTrail;

	int posX,posY;

public:
	void Initialize();
	Module* ExecuteModule();
	void Shutdown();
	void GiveData(void*);
};

extern Module* GesturesMod;

#endif