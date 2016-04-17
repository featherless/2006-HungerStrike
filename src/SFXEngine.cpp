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

#include "SFXEngine.h"
#include "GameDefs.h"

SFXEngine sfx;

const char SFXNames[NUMSFX][128]=
{
	"SFX/Fire Spell.ogg",
	"SFX/Storm Spell.ogg",
	"SFX/Punch.ogg",
	"SFX/Spell level 1.ogg",
	"SFX/Zombie Die 1.ogg",
	"SFX/Zombie Die 2.ogg",
	"SFX/Zombie Die 3.ogg",
	"SFX/Zombie Die 4.ogg",
	"SFX/Zombie Die 5.ogg",
	"SFX/Zombie Die 6.ogg",
	"SFX/Zombie Die 7.ogg",
	"SFX/Zombie Die 8.ogg",
	"SFX/Zombie Die 9.ogg",
	"SFX/Zombie Die 10.ogg",
	"SFX/Punch Hit.ogg",
	"SFX/Menu Browse.ogg",
	"SFX/Menu Select.ogg",
	"SFX/Eat This.ogg",
	"SFX/Game Lose.ogg",
	"SFX/Game Win.ogg",
	"SFX/Pillar.ogg",
	"SFX/Final Boss Laugh.ogg",
	"SFX/Final Boss Laugh 2.ogg",
	"SFX/Level 2 Boss Death.ogg",
	"SFX/Cannon.ogg",
	"SFX/Level 2 Boss Hit.ogg",
	"SFX/Final Boss Hit.ogg",
	"SFX/Final Boss Die.ogg",
	"SFX/Main character hit.ogg",
	"SFX/Zombie Explode.ogg",
	"SFX/Spell level 2.ogg",
};

void SFXEngine::LoadSFXs()
{
	for(UINT a=0;a<NUMSFX;a++)
	{
		game.PrintAndSwap("Loading Sound Effect: %s",SFXNames[a]);
		FMOD_RESULT result;
		result = fmodSystem->createStream((char*)SFXNames[a], FMOD_SOFTWARE | FMOD_LOOP_OFF | FMOD_2D, 0, &effects[a]);
		if (result != FMOD_OK)
			MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error loading sound effect!",L"Error",MB_OK);
	}
}

void SFXEngine::PlaySFX(unsigned int ID)
{
	if(effects[ID])
	{
		FMOD_RESULT result;
		result = fmodSystem->playSound(FMOD_CHANNEL_FREE, effects[ID], false, &Channels[ID]);
	}
}

void SFXEngine::stop(unsigned int ID)
{
	Channels[ID]->stop();
}

void SFXEngine::SetVolume(unsigned int ID,unsigned char volume)
{
	Channels[ID]->setVolume(((float)volume)/255);
}