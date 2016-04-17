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

#include "MainMenuModule.h"
#include "GameDefs.h"
#include "GameLevel1Module.h"
#include "GameLevel2Module.h"
#include "GameLevel3Module.h"
#include "InstructionsModule.h"
#include "CreditsModule.h"
#include "SFXEngine.h"

Module* MainMenuMod;

int difficulty=NORMAL;
int gesturedifficulty=EASY;

enum MainMenuStages { FADEIN, SELECTOPTION, CHANGEOPTIONS, SELECTLEVEL, FADEOUT };

void MainMenuModule::GiveData(void* data)
{}

void MainMenuModule::Initialize()
{
	parts.ShutdownEngine();

	FMOD_RESULT result;
    result = fmodSystem->createStream("music/Opening Theme.ogg", FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &bgm);
	if (result != FMOD_OK)
		MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error loading music/Opening Theme.ogg!",L"Error",MB_OK);

	result = fmodSystem->playSound(FMOD_CHANNEL_FREE, bgm, false, &bgmchannel);
	if (result != FMOD_OK)
		MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error playing music/Opening Theme.ogg!",L"Error",MB_OK);

	glEnable(GL_TEXTURE_2D);

	startTick=GetTickCount();
	stage=FADEIN;
	lastHover=-1;
}

Module* MainMenuModule::ExecuteModule()
{
	posX=(int)((mouse.X-game.newPosX));
	posY=(int)((600-(mouse.Y-game.newPosY)));

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
			diff=1;
			if(doThisAction==1)
				return GameLevel1Mod;
			else if(doThisAction==2)
				return CreditsMod;
			else if(doThisAction==3)
				return InstructionsMod;
			else if(doThisAction==4)
				return 0;
			else if(doThisAction==5)
				return GameLevel2Mod;
			else if(doThisAction==6)
				return GameLevel3Mod;
		}
		bgmchannel->setVolume(1-diff);
		glColor4f(1-diff,1-diff,1-diff,1-diff);
	}
	else
		glColor4f(1,1,1,1);
	SetTexture1(game.Textures[TITLE]);
	glLoadIdentity();
	glBegin(GL_QUADS);
		glTexCoord2f(0.78125f,0);			glVertex2f(800,600);
		glTexCoord2f(0,       0);			glVertex2f(0,  600);
		glTexCoord2f(0,       0.5859375f);	glVertex2f(0,    0);
		glTexCoord2f(0.78125f,0.5859375f);	glVertex2f(800,  0);
	glEnd();

	int doAction=0;
	if(mouse.L && !mouse.hL)
	{
		mouse.hL=true;

		doAction=1;
	}

	game.Print(550,575,0,"Hunger Strike/");
	game.Print(520,575-16,0,"Team GameDev Guys");



	if(stage<=SELECTOPTION || stage==FADEOUT)
	{
		int offset=390;
		if(posX>700 && posX<700+4*16 && posY<=offset-16 && posY>offset-32 && stage==SELECTOPTION)
		{
			glColor3f(1,0,0);

			if(doAction)
			{
				stage=SELECTLEVEL;
				sfx.PlaySFX(MENUSELECT);
				sfx.SetVolume(MENUSELECT,110);
			}
			if(lastHover!=0)
			{
				sfx.PlaySFX(MENUBROWSE);
				lastHover=0;
			}
		}
		else if(stage==SELECTOPTION)
			glColor3f(1,1,1);
		game.Print(700,offset-=32,0,"Play");

		if(posX>650 && posX<700+7*16 && posY<=offset-16 && posY>offset-32 && stage==SELECTOPTION)
		{
			glColor3f(1,0,0);

			if(doAction)
			{
				stage=CHANGEOPTIONS;
				sfx.PlaySFX(MENUSELECT);
				sfx.SetVolume(MENUSELECT,110);
			}

			if(lastHover!=1)
			{
				sfx.PlaySFX(MENUBROWSE);
				lastHover=1;
			}
		}
		else if(stage==SELECTOPTION)
			glColor3f(1,1,1);
		game.Print(660,offset-=32,0,"Options");

		if(posX>650 && posX<660+7*16 && posY<=offset-16 && posY>offset-32 && stage==SELECTOPTION)
		{
			glColor3f(1,0,0);
			if(doAction)
			{
				stage=FADEOUT;
				startTick=GetTickCount();
				doThisAction=2;
				sfx.PlaySFX(MENUSELECT);
				sfx.SetVolume(MENUSELECT,110);
			}
			if(lastHover!=2)
			{
				sfx.PlaySFX(MENUBROWSE);
				lastHover=2;
			}
		}
		else if(stage==SELECTOPTION)
			glColor3f(1,1,1);
		game.Print(660,offset-=32,0,"Credits");

		if(posX>764-strlen("Instructions")*16 && posX<764 && posY<=offset-16 && posY>offset-32 && stage==SELECTOPTION)
		{
			glColor3f(1,0,0);
			if(doAction)
			{
				stage=FADEOUT;
				startTick=GetTickCount();
				doThisAction=3;
				sfx.PlaySFX(MENUSELECT);
				sfx.SetVolume(MENUSELECT,110);
			}
			if(lastHover!=3)
			{
				sfx.PlaySFX(MENUBROWSE);
				lastHover=3;
			}
		}
		else if(stage==SELECTOPTION)
			glColor3f(1,1,1);
		game.Print(764-strlen("Instructions")*16,offset-=32,0,"Instructions");

		if(posX>700 && posX<700+4*16 && posY<=offset-16 && posY>offset-32 && stage==SELECTOPTION)
		{
			glColor3f(1,0,0);

			if(doAction)
			{
				stage=FADEOUT;
				startTick=GetTickCount();
				doThisAction=4;
				sfx.PlaySFX(MENUSELECT);
				sfx.SetVolume(MENUSELECT,110);
			}
			if(lastHover!=4)
			{
				sfx.PlaySFX(MENUBROWSE);
				lastHover=4;
			}
		}
		else if(stage==SELECTOPTION)
			glColor3f(1,1,1);
		game.Print(700,offset-=32,0,"Quit");
	}
	else if(stage==CHANGEOPTIONS)
	{
		int offset=390;
		glColor3f(1,1,1);
		game.Print(764-strlen("Options")*16,offset-=32,0,"Options");
		game.Print(764-strlen("Game Difficulty")*16,offset-=32,0,"Game Difficulty");

		int length=strlen(DifficultyLevelsText[difficulty])*16;
		int xoff=764-length;
		if(posX>xoff && posX<764 && posY<=offset-16 && posY>offset-32)
		{
			glColor3f(1,0,0);

			if(doAction)
			{
				difficulty++;
				difficulty%=NUMDIFFLEVELS;

				length=strlen(DifficultyLevelsText[difficulty])*16;
				xoff=764-length;
				sfx.PlaySFX(MENUSELECT);
				sfx.SetVolume(MENUSELECT,110);
			}
			if(lastHover!=5)
			{
				sfx.PlaySFX(MENUBROWSE);
				lastHover=5;
			}
		}
		else
			glColor3f(1,1,1);
		game.Print(xoff,offset-=32,0,"%s",DifficultyLevelsText[difficulty]);


		glColor3f(1,1,1);
		game.Print(764-strlen("Gesture Difficulty")*16,offset-=32,0,"Gesture Difficulty");

		length=strlen(DifficultyLevelsText[gesturedifficulty])*16;
		xoff=764-length;
		if(posX>xoff && posX<764 && posY<=offset-16 && posY>offset-32)
		{
			glColor3f(1,0,0);

			if(doAction)
			{
				gesturedifficulty++;
				gesturedifficulty%=NUMDIFFLEVELS;

				length=strlen(DifficultyLevelsText[gesturedifficulty])*16;
				xoff=764-length;
				sfx.PlaySFX(MENUSELECT);
				sfx.SetVolume(MENUSELECT,110);
			}
			if(lastHover!=6)
			{
				sfx.PlaySFX(MENUBROWSE);
				lastHover=6;
			}
		}
		else
			glColor3f(1,1,1);
		game.Print(xoff,offset-=32,0,"%s",DifficultyLevelsText[gesturedifficulty]);




		if(posX>700 && posX<700+4*16 && posY<=offset-16 && posY>offset-32)
		{
			glColor3f(1,0,0);

			if(doAction)
			{
				stage=SELECTOPTION;
				sfx.PlaySFX(MENUSELECT);
				sfx.SetVolume(MENUSELECT,110);
			}
			if(lastHover!=7)
			{
				sfx.PlaySFX(MENUBROWSE);
				lastHover=7;
			}
		}
		else
			glColor3f(1,1,1);
		game.Print(700,offset-=32,0,"Back");
	}
	else if(stage==SELECTLEVEL)
	{
		int offset=390;
		glColor3f(1,1,1);
		game.Print(764-strlen("Choose Level")*16,offset-=32,0,"Choose Level");

		if(posX>764-strlen("Level 1")*16 && posX<764 && posY<=offset-16 && posY>offset-32)
		{
			game.Print(200,250,0,"High Scores");
			for(int a=0;a<4;a++)
				game.Print(200,220-a*16,0,"%s %d",DifficultyLevelsText[a],gameDat.highScores1[a]);

			game.Print(450,250,0,"High Combos");
			for(int a=0;a<4;a++)
				game.Print(450,220-a*16,0,"%s %d",DifficultyLevelsText[a],gameDat.highCombos1[a]);

			glColor3f(1,0,0);

			if(doAction)
			{
				doThisAction=1;
				stage=FADEOUT;
				startTick=GetTickCount();
				sfx.PlaySFX(MENUSELECT);
				sfx.SetVolume(MENUSELECT,110);
			}
			if(lastHover!=8)
			{
				sfx.PlaySFX(MENUBROWSE);
				lastHover=8;
			}
		}
		else
			glColor3f(1,1,1);
		game.Print(764-strlen("Level 1")*16,offset-=32,0,"Level 1");

		if(gameDat.level>=1)
		{
			if(posX>764-strlen("Level 2")*16 && posX<764 && posY<=offset-16 && posY>offset-32)
			{
				glColor3f(1,0,0);

				if(doAction)
				{
					doThisAction=5;
					stage=FADEOUT;
					startTick=GetTickCount();
					sfx.PlaySFX(MENUSELECT);
					sfx.SetVolume(MENUSELECT,110);
				}
				if(lastHover!=9)
				{
					sfx.PlaySFX(MENUBROWSE);
					lastHover=9;
				}
			}
			else
				glColor3f(1,1,1);
			game.Print(764-strlen("Level 2")*16,offset-=32,0,"Level 2");
		}

		if(gameDat.level>=2)
		{
			if(posX>764-strlen("Level 3")*16 && posX<764 && posY<=offset-16 && posY>offset-32)
			{
				glColor3f(1,0,0);

				if(doAction)
				{
					doThisAction=6;
					stage=FADEOUT;
					startTick=GetTickCount();
					sfx.PlaySFX(MENUSELECT);
					sfx.SetVolume(MENUSELECT,110);
				}
				if(lastHover!=10)
				{
					sfx.PlaySFX(MENUBROWSE);
					lastHover=10;
				}
			}
			else
				glColor3f(1,1,1);
			game.Print(764-strlen("Level 3")*16,offset-=32,0,"Level 3");
		}



		if(posX>700 && posX<700+4*16 && posY<=offset-16 && posY>offset-32)
		{
			glColor3f(1,0,0);

			if(doAction)
			{
				stage=SELECTOPTION;
				sfx.PlaySFX(MENUSELECT);
				sfx.SetVolume(MENUSELECT,110);
			}
			if(lastHover!=7)
			{
				sfx.PlaySFX(MENUBROWSE);
				lastHover=7;
			}
		}
		else
			glColor3f(1,1,1);
		game.Print(700,offset-=32,0,"Back");
	}

	return MainMenuMod;
}

void MainMenuModule::Shutdown()
{
	bgmchannel->stop();
	bgm->release();
}
