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

#include "GameWinModule.h"
#include "GameDefs.h"
#include "MainMenuModule.h"
#include "SFXEngine.h"

Module* GameWinMod;

enum MainMenuStages { FADEIN, RUNNING, FADEOUT };

void GameWinModule::GiveData(void* data)
{}

void GameWinModule::Initialize()
{
	sfx.PlaySFX(GAMEWIN);
	startTick=GetTickCount();
	stage=FADEIN;
}

Module* GameWinModule::ExecuteModule()
{
	if(stage==FADEIN)
	{
		float diff=(float)(GetTickCount()-startTick)/3000;
		if(diff>1)
		{
			diff=1;
			stage=1;
			startTick=GetTickCount();
		}
		glColor4f(diff,diff,diff,diff);
	}
	else if(stage==FADEOUT)
	{
		float diff=(float)(GetTickCount()-startTick)/1000;
		if(diff>1)
		{
			stage++;
			diff=1;
		}
		sfx.SetVolume(GAMEWIN,(1-diff)*255);
		glColor4f(1-diff,1-diff,1-diff,1-diff);
	}
	else if(stage==RUNNING)
		glColor4f(1,1,1,1);

	game.Print(-1,0,0,"Press escape to return to the main menu.");

	SetTexture1(game.Textures[WIN]);
	glLoadIdentity();
	glBegin(GL_QUADS);
		glTexCoord2f(0.78125f,0);			glVertex2f(800,600);
		glTexCoord2f(0,       0);			glVertex2f(0,  600);
		glTexCoord2f(0,       0.5859375f);	glVertex2f(0,    0);
		glTexCoord2f(0.78125f,0.5859375f);	glVertex2f(800,  0);
	glEnd();

	if(keys[VK_ESCAPE] && !lkeys[VK_ESCAPE] && stage==RUNNING)
	{
		stage=FADEOUT;
		startTick=GetTickCount();
	}
	if(stage>FADEOUT)
		return MainMenuMod;
	return GameWinMod;
}

void GameWinModule::Shutdown()
{
	sfx.stop(GAMEWIN);
}
