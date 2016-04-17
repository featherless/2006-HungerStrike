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

#include "WinLevel2Module.h"
#include "GameDefs.h"
#include "GameLevel3Module.h"

Module* WinLevel2Mod;

void WinLevel2Module::GiveData(void* data)
{}

void WinLevel2Module::Initialize()
{
}

Module* WinLevel2Module::ExecuteModule()
{
	int offset=400;
	game.Print(-1,offset-=16,0,"As the cannon-wielding menace fell to");
	game.Print(-1,offset-=16,0,"the ground, it revealed the secret location");
	game.Print(-1,offset-=16,0,"of its boss...");
	game.Print(-1,offset-=48,0,"Score: %d",level1Score.score);
	game.Print(-1,offset-=48,0,"Press space to continue to level 3");

	if(keys[VK_SPACE] && !lkeys[VK_SPACE])
		return GameLevel3Mod;
	return WinLevel2Mod;
}

void WinLevel2Module::Shutdown()
{
}
