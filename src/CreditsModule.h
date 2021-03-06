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

#ifndef CREDITSMODULE_H_
#define CREDITSMODULE_H_

#include "Module.h"
#include "Zombie.h"

class CreditsModule:public Module
{
private:
	FMOD::Sound *bgm;
	FMOD::Channel *bgmchannel;

	Zombie zomb;

	unsigned long creditsTick;
	int stage;
public:
	void Initialize();
	Module* ExecuteModule();
	void Shutdown();
	void GiveData(void*);
};

extern Module* CreditsMod;

#endif