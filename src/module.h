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

#ifndef MODULE
#define MODULE

#include "ParticleSystem.h"
#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>


struct Mouse
{
	int X,Y;
	bool R,L,M;
	bool hR,hL,hM;
	short Pad;
};

extern Mouse mouse;

extern bool keys[256];
extern bool lkeys[256];

class Module
{
private:
public:
	virtual void Initialize()=0;
	virtual Module* ExecuteModule()=0;
	virtual void Shutdown()=0;
	virtual void GiveData(void*)=0;
};

extern int NoReset;				// Used in the module system to give the modules more control over how things
extern int NoInit;				// are initialized and shutdown.
extern int NoSubInit;

extern ParticleEngineServer parts;

extern Module* lastMod;

extern FMOD::System *fmodSystem;

#endif