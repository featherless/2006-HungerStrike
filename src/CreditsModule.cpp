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

#include "CreditsModule.h"
#include "GameDefs.h"
#include "MainMenuModule.h"

Module* CreditsMod;

void CreditsModule::GiveData(void* data)
{}

void CreditsModule::Initialize()
{
	parts.ShutdownEngine();
	FMOD_RESULT result;
    result = fmodSystem->createStream("music/Credits.ogg", FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &bgm);
	if (result != FMOD_OK)
		MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error loading music/Credits.ogg!",L"Error",MB_OK);

	result = fmodSystem->playSound(FMOD_CHANNEL_FREE, bgm, false, &bgmchannel);
	if (result != FMOD_OK)
		MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error playing music/Credits.ogg!",L"Error",MB_OK);
	bgmchannel->setVolume(1);

	creditsTick=GetTickCount();
	stage=0;

	zomb.createRandom();
	if(rand()%100<50)
		zomb.x=-300;
	else
		zomb.x=1100;
}

Module* CreditsModule::ExecuteModule()
{
	float diff=(float)(GetTickCount()-creditsTick)/2000;

	glEnable(GL_BLEND);
	glColor4f(1,1,1,sinf(diff*3.14159f));
	if(stage==0)
		game.Print(-1,300,0,"Hunger Strike/");
	else if(stage==1)
		game.Print(-1,300,0,"Team GameDev Guys: March 2006");
	else if(stage==2)
		game.Print(-1,300,0,"Created for the 72 Hour GDC");
	else if(stage==3)
		game.Print(-1,300,0,"Programmer:");
	else if(stage==4)
		game.Print(-1,300,0,"Jeff Verkoeyen");
	else if(stage==5)
		game.Print(-1,300,0,"Composer:");
	else if(stage==6)
		game.Print(-1,300,0,"Josh Rodasti");
	else if(stage==7)
		game.Print(-1,300,0,"Artist:");
	else if(stage==8)
		game.Print(-1,300,0,"Morganne Sparks");
	else
		stage=0;
	if(diff>1)
	{
		diff=1;
		stage++;
		creditsTick=GetTickCount();
	}
	if(fabsf(zomb.x-400)<50 && !zomb.isOnFire())
		zomb.ignite();
	glColor4f(1,1,1,1);
	zomb.process(1);
	zomb.render(1);

	parts.RenderAll(1);

	if(keys[VK_ESCAPE] && !lkeys[VK_ESCAPE])
		return MainMenuMod;
	return CreditsMod;
}

void CreditsModule::Shutdown()
{
	parts.ShutdownEngine();
	bgmchannel->stop();
}
