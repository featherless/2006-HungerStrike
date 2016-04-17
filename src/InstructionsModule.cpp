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

#include "InstructionsModule.h"
#include "GameDefs.h"
#include "MainMenuModule.h"

Module* InstructionsMod;

void InstructionsModule::GiveData(void* data)
{}

void InstructionsModule::Initialize()
{
	stage=0;
}

Module* InstructionsModule::ExecuteModule()
{
	glColor4f(1,1,1,1);
	game.Print(-1,600-16,0,"Instructions");

	if(stage==0)
	{
		int offset=568;
		game.Print(0,offset-=16,0,"Controls: (Level 1)");
		game.Print(0,offset-=16,0,"A  Move left");
		game.Print(0,offset-=16,0,"D  Move right");
		game.Print(0,offset-=16,0,"S   Punches");

		game.Print(0,offset-=32,0,"Controls: (Level 2)");
		game.Print(0,offset-=16,0,"W  Move up");
		game.Print(0,offset-=16,0,"S  Move down");

		game.Print(0,offset-=32,0,"Controls: (Every Level)");
		game.Print(0,offset-=16,0,"Space bar  Bring up Gesture Panel");

		game.Print(0,offset-=32,0,"The goal of the game is to survive each level.  We");
		game.Print(0,offset-=16,0,"won't spoil the plot line, but be aware that you");
		game.Print(0,offset-=16,0,"will have to cast spells in every level using a");
		game.Print(0,offset-=16,0,"Gesture system.");

		game.Print(0,offset-=32,0,"Press space to continue through this information");
	}
	else if(stage==1)
	{
		int offset=568;
		game.Print(0,offset-=16,0,"When you press space, you are shown the gestures");
		game.Print(0,offset-=16,0,"menu (shown below).  Once this menu is up, the");
		game.Print(0,offset-=16,0,"game pauses and you can move your mouse over your");
		game.Print(0,offset-=16,0,"spells to see the gesture required to do that");
		game.Print(0,offset-=16,0,"spell.");
		game.Print(0,offset-=32,0,"Once you have watched the spell, click the spell");
		game.Print(0,offset-=16,0,"icon you wish to cast to begin doing the gesture.");
		game.Print(0,offset-=32,0,"To cast the gesture, click and drag from the");
		game.Print(0,offset-=16,0,"starting position (drawn as a crystal) through the");
		game.Print(0,offset-=16,0,"path that was drawn out for you.");

		glLoadIdentity();
		glTranslatef(144,0,0);
		SetTexture1(game.Textures[INSTRUCTIONS1]);
		glBegin(GL_QUADS);
			glTexCoord2f(1,0);			 glVertex2f(512,367);
			glTexCoord2f(0,0);			 glVertex2f(0,  367);
			glTexCoord2f(0,0.716796875f);glVertex2f(0,    0);
			glTexCoord2f(1,0.716796875f);glVertex2f(512,  0);
		glEnd();
	}
	else if(stage==2)
	{
		int offset=568;
		game.Print(0,offset-=16,0,"When casting the spell, you can be sure you've hit");
		game.Print(0,offset-=16,0,"the area that you want as the icon for that");
		game.Print(0,offset-=16,0,"direction will disappear.");

		game.Print(0,offset-=32,0,"Take some time to get used to the gesture system");
		game.Print(0,offset-=16,0,"in the first level as it is KEY to playing this");
		game.Print(0,offset-=16,0,"game/");

		game.Print(0,offset-=32,0,"Press escape to return to the main menu.");

		glLoadIdentity();
		glTranslatef(144,0,0);
		SetTexture1(game.Textures[INSTRUCTIONS1]);
		glBegin(GL_QUADS);
			glTexCoord2f(1,0);			 glVertex2f(512,367);
			glTexCoord2f(0,0);			 glVertex2f(0,  367);
			glTexCoord2f(0,0.716796875f);glVertex2f(0,    0);
			glTexCoord2f(1,0.716796875f);glVertex2f(512,  0);
		glEnd();
	}

	if(keys[VK_SPACE] && !lkeys[VK_SPACE] && stage<2)
	{
		stage++;
	}

	if(keys[VK_ESCAPE] && !lkeys[VK_ESCAPE])
		return MainMenuMod;
	return InstructionsMod;
}

void InstructionsModule::Shutdown()
{
}
