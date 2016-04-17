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

#include "GameLevel1Module.h"
#include "GameDefs.h"
#include "LoseModule.h"
#include "WinLevel1Module.h"
#include "SFXEngine.h"
#include "MainMenuModule.h"

#define LEVELLENGTH		(120)

enum Level1Stages { FADEIN, GUITARWAIT, FALLINGFIRE, EXPLODEFIRE, SAYTEXT, HEROFLYIN, EXPLODEFIRE2, FLYBACKCENTER, GAME };

Module* GameLevel1Mod;

#define GESTURESIZE		32

levelScores level1Score;
Directions* Fireball;
Directions* RainSpell;
const Directions* FireballSpells[] = { FireballEasy, FireballNormal, FireballHard, FireballCrazy };
const Directions* RainSpellSpells[] = { RainSpellEasy, RainSpellNormal, RainSpellHard, RainSpellCrazy };

void GameLevel1Module::GiveData(void* data)
{}

const int zombieDiffs[NUMDIFFLEVELS]={ 10, 15, 20, 25 };

void GameLevel1Module::Initialize()
{
	level1Score.comboMax=0;
	level1Score.score=0;
	numZombiesDiff=zombieDiffs[difficulty];
	Fireball=(Directions*)FireballSpells[gesturedifficulty];
	RainSpell=(Directions*)RainSpellSpells[gesturedifficulty];
	parts.ShutdownEngine();

	FMOD_RESULT result;
    result = fmodSystem->createStream("music/Level 1.ogg", FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &bgm);
	if (result != FMOD_OK)
		MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error loading music/Level 1.ogg!",L"Error",MB_OK);

    result = fmodSystem->createStream("music/Level 1 Intro.ogg", FMOD_SOFTWARE | FMOD_LOOP_OFF | FMOD_2D, 0, &bgmintro);
	if (result != FMOD_OK)
		MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error loading music/Level 1 Intro.ogg!",L"Error",MB_OK);

	burst=new Burst;
	burst->SetNumParts(100);
	burst->SetGravity(0.2f);
	burst->SetColor(255,0,0);
	parts.giveChild(burst,&partEngIDs[0]);

	explosion=new Explosion;
	explosion->SetNumParts(500);
	explosion->SetGravity(0.2f);
	explosion->SetColor(255,0,0);
	parts.giveChild(explosion,&partEngIDs[1]);

	shot.setPartID(partEngIDs[0]);
	shot.setExplodeID(partEngIDs[1]);

	RainData rainData;
	rainData.radius=1;
	rainData.life=3000;
	rainData.size=8;
	rain=new Rain;
	rain->SetNumParts(2000);
	rain->SetGravity(0.2f);
	rain->SetColor(0,0,255);
	rain->GiveData(&rainData);
	rain->SetStartVect(&levelOffsetVect);
	parts.giveChild(rain,&partEngIDs[2]);

	clickStart = Vector(0,0,0);


	GestureTrailData data;
	data.radius=1;
	data.size=16;
	gestureTrail=new GestureTrail;
	gestureTrail->SetNumParts(500);
	gestureTrail->SetColor(0,255,0);
	gestureTrail->GiveData(&data);
	gestureTrail->SetStartVect(&clickStart);
	parts.giveChild(gestureTrail,&gestureTrailID);

	wiz.x = 400;
	wiz.y = 160;
	wiz.xv= 0;
	wiz.facing=1;
	wiz.hover=0;
	wiz.score=0;
	wiz.kills=0;
	levelOffset=mountainOffset=0;

	spell = (Directions*)Fireball;

// Animation INIT
	punching=0;
	punchingTimer=GetTickCount();


	lastTick = GetTickCount();

// Set to non-negative-one to start the spell showing
	showingSpell=0;
	showSpellTimer=GetTickCount();

	doGesture=0;
	showMenu=0;

	wiz.health=100;

	for(int a=0;a<numZombiesDiff;a++)
	{
		zombies[a].score=&wiz.score;
		zombies[a].createRandom();
	}

	wiz.injuredTick=GetTickCount();
	punchingTimer=GetTickCount();

	head=curr=0;

	combo=0;

	result = fmodSystem->playSound(FMOD_CHANNEL_FREE, bgmintro, false, &bgmintrochannel);
	if (result != FMOD_OK)
		MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error playing music/Level 1 Intro.ogg!",L"Error",MB_OK);

	bgmchannel=false;

	for(int a=0;a<4;a++)
	{
		introZombies[a].createRandom();
		introZombies[a].moving=0;
	}
	introZombies[0].x=360;
	introZombies[1].x=440;
	introZombies[2].x=400;
	introZombies[3].x=700;
	introZombies[3].facing=0;
	stage=FADEIN;
	levelBeginTick=GetTickCount();
	rainSlowTick=0;

	bossTick=GetTickCount();
	bossAnimation=0;
}

Module* GameLevel1Module::ExecuteModule()
{
	bool isPlaying=false;
	posX=(int)((mouse.X-game.newPosX));
	posY=(int)((600-(mouse.Y-game.newPosY)));
	levelOffsetVect=Vector(levelOffset,0,0);

	if(keys[VK_ESCAPE] && !lkeys[VK_ESCAPE] && stage!=GAME)
	{
		stage=GAME;
		levelBeginTick=GetTickCount();

		for(int a=0;a<4;a++)
			introZombies[a].killFire();
		shot.shooting=0;
		wiz.x=400;
		bgmintrochannel->stop();
		FMOD_RESULT result;
		result = fmodSystem->playSound(FMOD_CHANNEL_FREE, bgm, false, &bgmchannel);
		if (result != FMOD_OK)
			MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error playing music/Level 1.ogg!",L"Error",MB_OK);
	}
	else if(keys[VK_ESCAPE] && !lkeys[VK_ESCAPE])
	{
		if(showMenu)
		{
			showMenu=!showMenu;
			if(!showMenu)
				doGesture=false;
		}
		else
			return MainMenuMod;
	}

	unsigned int songTime=0;
// Play song
	bgmintrochannel->isPlaying(&isPlaying);
	if(!isPlaying && !bgmchannel)
	{
		FMOD_RESULT result;
		result = fmodSystem->playSound(FMOD_CHANNEL_FREE, bgm, false, &bgmchannel);
		if (result != FMOD_OK)
			MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error playing music/Level 1.ogg!",L"Error",MB_OK);

		stage=GAME;
		levelBeginTick=GetTickCount();
	}
	else
	{
		bgmintrochannel->getPosition(&songTime,FMOD_TIMEUNIT_MS);
	}
	int timeDiff = LEVELLENGTH-(GetTickCount()-levelBeginTick)/1000;

	if(stage==GAME)
	{
		input();
		coldet();
		render();
	}

// Guitar fade in at 14600'ish
	float scale;
	if(stage==FADEIN)
	{
		scale=((float)songTime)/2000;
		glColor4f(scale,scale,scale,scale);
		if(scale>=1.0f)
		{
			stage++;
			glColor4f(1,1,1,1);
		}
	}
	else
	{
		glColor4f(1,1,1,1);
		scale=1;
	}

	glEnable(GL_BLEND);
// Tiled ground
	SetTexture1(game.Textures[GRASS]);
	for(int x=((int)levelOffset)%64-64;x<=800+((int)levelOffset)%64+64;x+=64)
	{
		glPushMatrix();
			glTranslatef((float)x,32,-5);

			glBegin(GL_QUADS);
				glTexCoord2f(1,0.0f); glVertex2f( 32, 20);
				glTexCoord2f(0,0.0f); glVertex2f(-32, 20);
				glTexCoord2f(0,1.0f); glVertex2f(-32,-32);
				glTexCoord2f(1,1.0f); glVertex2f( 32,-32);

				glTexCoord2f(1,0.0f); glVertex2f( 32, 60);
				glTexCoord2f(0,0.0f); glVertex2f(-32, 60);
				glTexCoord2f(0,1.0f); glVertex2f(-32, 20);
				glTexCoord2f(1,1.0f); glVertex2f( 32, 20);
			glEnd();
		glPopMatrix();
	}

// Mountains
	SetTexture1(game.Textures[MOUNTAIN]);
	for(int x=((int)mountainOffset)%256-256;x<=800+((int)mountainOffset)%256+256;x+=256)
	{
		glPushMatrix();
			glTranslatef((float)x,188,-5);

			glBegin(GL_QUADS);
				glTexCoord2f(1,0.0f); glVertex2f( 128, 128);
				glTexCoord2f(0,0.0f); glVertex2f(-128, 128);
				glTexCoord2f(0,1.0f); glVertex2f(-128,-128);
				glTexCoord2f(1,1.0f); glVertex2f( 128,-128);
			glEnd();
		glPopMatrix();
	}

	if(stage==GAME)
	{
		render2();

		update();
		glColor4f(1,1,1,1);
	}

	if(stage<EXPLODEFIRE)
	{
		for(int a=0;a<4;a++)
		{
			introZombies[a].process(1);
			introZombies[a].render(scale);
		}
	}
	if(stage==GUITARWAIT)
	{
		float timeDiff=(float)(songTime-2000)/11000;
		game.Print(-1,432,0,"And so our story begins with a town of");
		game.Print(-1,400,0,"zombies...");

		if(songTime>=5000)
			game.Print(-1,360,0,"....and a sandwich.");

		glPushMatrix();
		glTranslatef(timeDiff*1120-256,150,5);
		SetTexture1(game.Textures[BOSS2]);
		if(GetTickCount()-bossTick>100)
		{
			bossAnimation++;
			bossAnimation%=5;
			bossTick=GetTickCount();
		}
		float tx,ty;
		tx=(float)(bossAnimation%4)*0.25f;
		ty=(float)((int)(bossAnimation/4))*0.5f;

		glColor4f(1,1,1,1);
		glBegin(GL_QUADS);
			glTexCoord2f(tx,ty); glVertex2f( 64, 128);
			glTexCoord2f(tx+0.25f,	 ty); glVertex2f(-64, 128);
			glTexCoord2f(tx+0.25f,	 ty+0.5f); glVertex2f(-64,-128);
			glTexCoord2f(tx,ty+0.5f); glVertex2f( 64,-128);
		glEnd();
		glPopMatrix();
	}
	if(stage==GUITARWAIT && songTime>13000)
	{
		stage++;
		shot.setPos(Vector(400,650,0));
		shot.setVel(Vector(0,0,0));
		shot.shoot();
	}
	else if(stage==FALLINGFIRE)
	{
		float timeDiff=(float)(songTime-13000)/1460;
		if(timeDiff>1)
		{
			timeDiff=1;
			stage++;
			shot.explode();
			for(int a=0;a<3;a++)
				introZombies[a].ignite();
		}
		else
			shot.setPosNoKill(Vector(400,(1-timeDiff)*500+140,0));
		shot.render(1);
	}
	else if(stage==EXPLODEFIRE)
	{
		introZombies[3].animation=0;
		for(int a=0;a<3;a++)
		{
			introZombies[a].process(1);
			introZombies[a].render(1);
		}
		introZombies[3].render(1);

		if(songTime>=16000)
		{
			for(int a=0;a<3;a++)
				introZombies[a].killFire();
			stage++;
		}
	}
	else if(stage==SAYTEXT)
	{
		introZombies[3].render(1);
		game.Print(550,300,0,"HURHG??");
		game.Print(0,350,0,"GIVE ME MY SANDWICH//");

		if(songTime>=19000)
			stage++;
	}
	else if(stage==HEROFLYIN)
	{
		float timeDiff=(float)(songTime-19000)/1000;

		if(timeDiff>1)
		{
			timeDiff=1;
			stage++;
			introZombies[3].ignite();
		}
		if(timeDiff>=0.7f && !punching)
		{
			punching=1;
			punchingTimer=GetTickCount();
			sfx.PlaySFX(PUNCH);
		}
		introZombies[3].render(1);
		wiz.x=timeDiff*600;
		render();
	}
	else if(stage==EXPLODEFIRE2)
	{
		introZombies[3].process(1);
		introZombies[3].animation=0;
		introZombies[3].render(1);
		wiz.x=600;
		render();
		if(songTime>=21500)
		{
			introZombies[3].killFire();
			stage++;
		}
	}
	else if(stage==FLYBACKCENTER)
	{
		unsigned int length;
		bgmintro->getLength(&length,FMOD_TIMEUNIT_MS);
		float timeLeft=length-songTime;
		float timeOverall=length-21500;
		float diffTime=1-timeLeft/timeOverall;

		wiz.x=600-200*diffTime;
		render();
		game.Print(-1,432,0,"heh...heh heh....heh heh heh heh...hah");
		game.Print(-1,400,0,"that sandwich ought to be around here somewhere...");
	}

	glDisable(GL_BLEND);
// Text and stuff
	//game.Print(0,16,0,"%f",fps);

	if(difficulty<=NORMAL && stage==GAME && timeDiff>LEVELLENGTH-10)
	{
		glColor3f(1,1,1);
		game.Print(-1,16,0,"Press A and D to move, S to punch");
		game.Print(-1,0,0,"Press space to do a gesture");
	}

	shot.render(!showMenu);

	parts.RenderIndex(gestureTrailID,1);

	parts.RenderAll(!showMenu);

	if(wiz.health<=10 && stage==GAME)	return LoseMod;
	if(timeDiff<=0 && stage==GAME)		return WinLevel1Mod;

	return GameLevel1Mod;
}

void GameLevel1Module::input()
{
// Movement and punching
	if(!showMenu)
	{
		if(keys['D'])
		{
			wiz.xv=-3;
			wiz.facing = 1;
		}
		if(keys['A'])
		{
			wiz.xv=3;
			wiz.facing = 0;
		}
		if(keys['A']==keys['D'])
			wiz.xv=0;
		if(keys['S'] && !lkeys['S'] && !punching)
		{
			punching=1;
			punchingTimer=GetTickCount();
			sfx.PlaySFX(PUNCH);
		}
	}

// Bringing up the spell menu
	if(!shot.isShooting() && keys[VK_SPACE] && !lkeys[VK_SPACE])
	{
		spell=0;

		lastTick = GetTickCount();

		clickedDirection=INVALID;
		index=0;
		showingSpell=0;
		showSpellTimer=GetTickCount();

		doGesture=true;
		showMenu=!showMenu;
		if(!showMenu)
			doGesture=false;

		menuLastTick=GetTickCount();
	}
}

void GameLevel1Module::coldet()
{
	int timeDiff = LEVELLENGTH-(GetTickCount()-levelBeginTick)/1000;
// Collision detection with zombies
	if(shot.isShooting())
	{
		Vector shotPos=shot.getPos();

		for(int a=0;a<numZombiesDiff*((float)(LEVELLENGTH-timeDiff)/LEVELLENGTH);a++)
		{
			if(zombies[a].isAlive() && shotPos.x>zombies[a].x-32 && shotPos.x<=zombies[a].x+32)
			{
				for(int b=0;b<numZombiesDiff*((float)(LEVELLENGTH-timeDiff)/LEVELLENGTH);b++)
				{
					if(a!=b && zombies[b].isAlive() && shotPos.x>zombies[b].x-64 && shotPos.x<=zombies[b].x+64)
					{
						zombies[b].ignite();		// COMBO!
						combo++;
					}
				}
				shot.explode();

				zombies[a].ignite();
				break;
			}
		}
	}

	int anyOnFire=0;
// Chaining zombies on fire
	for(int a=0;a<numZombiesDiff*((float)(LEVELLENGTH-timeDiff)/LEVELLENGTH);a++)
	{
		if(zombies[a].isOnFire())
		{
			for(int b=0;b<numZombiesDiff*((float)(LEVELLENGTH-timeDiff)/LEVELLENGTH);b++)
			{
				if(a!=b && zombies[b].isAlive() && !zombies[b].isOnFire() && fabsf(zombies[a].x-zombies[b].x)<64)
				{
					zombies[b].ignite();		// COMBO!
					combo++;
				}
			}
			anyOnFire=1;
		}
		else if(fabsf(zombies[a].x-wiz.x)>1600)
			zombies[a].createRandom();
		else if(fabsf(zombies[a].x-wiz.x)<48 && zombies[a].isAlive() && GetTickCount()>wiz.injuredTick)
		{
			wiz.injuredTick=GetTickCount()+1000;
			wiz.health-=25;

			sfx.PlaySFX(MAINCHARHIT);
		}
	}

	if(!anyOnFire && combo)
	{
		if(combo)
		{
			if(combo>level1Score.comboMax)
				level1Score.comboMax=combo;
			scoreText* newScore = new scoreText;

			newScore->x=400;
			newScore->y=520;
			newScore->next=0;
			newScore->score=combo*50;
			newScore->timer=GetTickCount();

			if(!head)
				head=curr=newScore;
			else
			{
				curr->next=newScore;
				curr=newScore;
			}

			wiz.score+=combo*50;
		}
		combo=0;
	}
}

void GameLevel1Module::render()
{
	int timeDiff = LEVELLENGTH-(GetTickCount()-levelBeginTick)/1000;
// Rendering the gestures
	if(doGesture)	renderGestures();

// Color the background if the menu's displayed
	if(showMenu)	glColor4f(0.5f,0.5f,0.5f,1);
	else			glColor4f(1,1,1,1);

// Render the wizard
	glLoadIdentity();
	SetTexture1(game.Textures[WIZARD]);
	glPushMatrix();
		glTranslatef(wiz.x,wiz.y,-1);

// Handling punching animations
		float offset=0;
		if(punching)
		{
			float diff=(float)(GetTickCount()-punchingTimer)/500;
			if(diff<0.2f)			offset=0;
			else if(diff<0.5f)		offset=0.25f;
			else if(diff<0.75f)		offset=0.5f;
			else if(diff<1.0f)
			{
				if(punching==3)
				{
					if(spell==(Directions*)Fireball && rainSlowTick<GetTickCount())
					{
						sfx.PlaySFX(FIRESPELL);
						if(wiz.facing)
						{
							shot.setVel(Vector(5-wiz.xv,0,0));
							shot.setPos(Vector(wiz.x+50,wiz.y+30,0));
						}
						else
						{
							shot.setVel(Vector(-5-wiz.xv,0,0));
							shot.setPos(Vector(wiz.x-50,wiz.y+30,0));
						}
						shot.shoot();
					}
					else if(spell==(Directions*)RainSpell)
					{
						sfx.PlaySFX(STORMSPELL);
						rain->ForceActivateParts(10000);
						rain->GiveData(0);
						rainSlowTick=GetTickCount()+3000;
					}
					punching=4;
				}
				offset=0.75f;
			}
			else					punching=0;

			if(offset>=0.5f && punching==1 && stage==GAME)
			{
				for(int a=0;a<numZombiesDiff*((float)(LEVELLENGTH-timeDiff)/LEVELLENGTH);a++)
				{
					if(!zombies[a].isPunched() && (zombies[a].x>400 && zombies[a].x<500 && wiz.facing ||
					zombies[a].x<400 && zombies[a].x>300 && !wiz.facing))
					{
						zombies[a].punch();
						punching=2;
						break;
					}
				}
			}
		}

	glEnable(GL_BLEND);
// Render the wizard
		int render=1;
		if(GetTickCount()<wiz.injuredTick)
		{
			int diff=(wiz.injuredTick-GetTickCount())%26;
			if(diff<13)
				render=0;
		}
		if(render)
		{
			wiz.hover+=g_speed/10;
			glTranslatef(0,sinf(wiz.hover)*5,0);
			glBegin(GL_QUADS);
				glTexCoord2f(wiz.facing*0.25f+offset,0.0f); glVertex2f( 64, 128);
				glTexCoord2f((1-wiz.facing)*0.25f+offset,0.0f); glVertex2f(-64, 128);
				glTexCoord2f((1-wiz.facing)*0.25f+offset,1.0f); glVertex2f(-64,-128);
				glTexCoord2f(wiz.facing*0.25f+offset,1.0f); glVertex2f( 64,-128);
			glEnd();
		}
	glPopMatrix();
}

void GameLevel1Module::render2()
{
	int timeDiff = LEVELLENGTH-(GetTickCount()-levelBeginTick)/1000;
// Health bar
	SetTexture1(game.Textures[HEALTH]);
	glPushMatrix();
		glTranslatef(750,536,10);

		float health=(float)(wiz.health)/100;
		glBegin(GL_QUADS);
			glTexCoord2f(1,   1-health); glVertex2f( 32, 64*health);
			glTexCoord2f(0,1-health); glVertex2f(-32, 64*health);
			glTexCoord2f(0,1.0f); glVertex2f(-32,  0);
			glTexCoord2f(1,   1.0f); glVertex2f( 32,  0);
		glEnd();
	glPopMatrix();

// Zombies
	for(int a=0;a<numZombiesDiff*((float)(LEVELLENGTH-timeDiff)/LEVELLENGTH);a++)
		zombies[a].render(1);
}

void GameLevel1Module::update()
{
	int timeDiff = LEVELLENGTH-(GetTickCount()-levelBeginTick)/1000;
// Update the game
	if(!showMenu)
	{
		levelOffset+=wiz.xv*g_speed;
		mountainOffset+=wiz.xv*g_speed/2;
		shot.offset(Vector(wiz.xv*g_speed,0,0));
		for(int a=0;a<numZombiesDiff*((float)(LEVELLENGTH-timeDiff)/LEVELLENGTH);a++)
		{
			zombies[a].update(wiz.xv*g_speed);
			float zombX=zombies[a].x;
			float scale=1;
			if(GetTickCount()<rainSlowTick)
				scale=0.2f;
			int score=zombies[a].process((float)(!showMenu)*scale);

// The zombie died!
			if(score)
			{
				wiz.kills++;
				scoreText* newScore = new scoreText;

				newScore->x=zombX;
				newScore->y=180;
				newScore->next=0;
				newScore->score=score;
				newScore->timer=GetTickCount();

				if(!head)
					head=curr=newScore;
				else
				{
					curr->next=newScore;
					curr=newScore;
				}
			}
		}
/*		wiz.x-=wiz.xv*g_speed;
		if(wiz.x>500)
		{
			levelOffset-=wiz.x-500;
			wiz.x=500;
		}
		else if(wiz.x<300)
		{
			levelOffset+=300-wiz.x;
			wiz.x=300;
		}*/

		shot.update(1);
		glColor4f(1,1,1,1);
	}
	else
	{
		unsigned long tickDiff = GetTickCount()-menuLastTick;
		levelBeginTick+=tickDiff;
		menuLastTick=GetTickCount();

		glLoadIdentity();

		glColor4f(1,1,1,1);

		SetTexture1(game.Textures[FIREBALLICON]);
		glPushMatrix();
			glTranslatef(400,568,5);

// Initializing the Gestures System
			glColor3f(1,1,1);
			if(posX>=400-32 && posX<=400+32 &&
			posY>=536 && posY<=600)
			{
				if(spell!=(Directions*)Fireball)
				{
					spell=(Directions*)Fireball;

					lastTick = GetTickCount();

					clickedDirection=INVALID;
					index=0;
					showingSpell=0;
					showSpellTimer=GetTickCount();


					if(combo)
					{
						scoreText* newScore = new scoreText;

						newScore->x=400;
						newScore->y=520;
						newScore->next=0;
						newScore->score=combo*50;
						newScore->timer=GetTickCount();

						if(!head)
							head=curr=newScore;
						else
						{
							curr->next=newScore;
							curr=newScore;
						}

						wiz.score+=combo*50;
					}
					combo=0;
				}

				glColor3f(1,0,0);

// Click!
				if(mouse.L && !mouse.hL)
				{
					mouse.hL=true;

					showMenu=0;

					showingSpell=-1;

					gestureTick=GetTickCount();

					spellTime=3000;
				}
			}
// Render fire spell image
		glEnable(GL_BLEND);
			glBegin(GL_QUADS);
				glTexCoord2f(1,0); glVertex2f( 32, 32);
				glTexCoord2f(0,0); glVertex2f(-32, 32);
				glTexCoord2f(0,1); glVertex2f(-32,-32);
				glTexCoord2f(1,1); glVertex2f( 32,-32);
			glEnd();
		glPopMatrix();

		SetTexture1(game.Textures[RAIN]);
		glPushMatrix();
			glTranslatef(464,568,5);

			glColor3f(1,1,1);
			if(posX>=464-32 && posX<=464+32 &&
			posY>=536 && posY<=600)
			{
				if(spell!=(Directions*)RainSpell)
				{
					spell=(Directions*)RainSpell;

					lastTick = GetTickCount();

					clickedDirection=INVALID;
					index=0;
					showingSpell=0;
					showSpellTimer=GetTickCount();
				}

				glColor3f(1,0,0);

// Click!
				if(mouse.L && !mouse.hL)
				{
					mouse.hL=true;

					showMenu=0;

					showingSpell=-1;

					gestureTick=GetTickCount();

					spellTime=4000;
				}
			}

// Render rain spell image
		glEnable(GL_BLEND);
			glBegin(GL_QUADS);
				glTexCoord2f(1,0); glVertex2f( 32, 32);
				glTexCoord2f(0,0); glVertex2f(-32, 32);
				glTexCoord2f(0,1); glVertex2f(-32,-32);
				glTexCoord2f(1,1); glVertex2f( 32,-32);
			glEnd();
		glPopMatrix();
		glDisable(GL_BLEND);
	}
// Score rendering
	scoreText* iter=head;
	scoreText* last=0;
	while(iter)
	{
		float diff=(float)(GetTickCount()-iter->timer)/1000;
		if(diff>1)
		{
			if(!last)
			{
				scoreText* next=head->next;
				delete head;
				head=next;
				iter=head;
				continue;
			}
			else if(iter==curr)
			{
				curr=last;
				delete iter;
				break;
			}
			else
			{
				last->next=iter->next;
				delete iter;
				iter=last;
			}
		}
		else
		{
			glColor4f(1,1,1,1-diff);
			iter->x+=wiz.xv*g_speed;
			game.Print((int)iter->x,(int)(iter->y + diff*80),0,"%d",iter->score);
		}


		last=iter;
		iter=iter->next;
	}

	glColor4f(1,1,1,1);
	if(combo)
		game.Print(400,600-32,0,"Combo: %d",combo+1);

	game.Print(0,568,0,"Score: %d",wiz.score);
	game.Print(0,536,0,"Kills: %d",wiz.kills);
	game.Print(0,504,0,"Time Left: %d",timeDiff);
}

void GameLevel1Module::renderGestures()
{
	Directions highlight = INVALID;
	Directions next = INVALID;
	float offset;

	if(showingSpell==-1 && !showMenu && spell)
	{
		if(difficulty<=NORMAL)
		{
			glColor3f(1,1,1);
			game.Print(-1,340,0,"Click and drag between icons/");
		}
		highlight = spell[index];
		next	  = spell[index+1];

		if(GetTickCount()-gestureTick>spellTime)
		{
			doGesture = 0;
			clickedDirection = INVALID;
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
			glPushMatrix();
				glLoadIdentity();
				glTranslatef(120,480,10);

				glColor4f(1,1,1,1);
				float diff=GetTickCount()-gestureTick;
				diff/=(float)spellTime;

				glBegin(GL_LINE_LOOP);
					glVertex2f(100,0);
					for(float r=0;r<(1-diff)*(3.14159f*2);r+=(3.14159f/20))
						glVertex2f(cos(r)*100,sin(r)*100);
					glVertex2f(0,0);
				glEnd();
			glPopMatrix();

			glEnable(GL_TEXTURE_2D);
		}

		if(mouse.L)
		{
			float closest = VectDist(Vector((float)posX,(float)posY,0),(Vector)dirVectors[TOP]+Vector(400,300,0));
			Directions closestDir = TOP;
			for(int a=1;a<BREAK;a++)
			{
				float dist = VectDist(Vector((float)posX,(float)posY,0),(Vector)dirVectors[a]+Vector(400,300,0));
				if(dist < closest)
				{
					closestDir = (Directions)a;
					closest = dist;
				}
			}
			if(!mouse.hL)
			{
				mouse.hL=true;
				clickedDirection=closestDir;
			}
			else
			{
				if(clickedDirection == highlight && closestDir == next)
				{
					index++;
					clickedDirection = next;
					if(spell[index+1] == END)
					{
						clickedDirection = INVALID;
						doGesture = 0;

						sfx.PlaySFX(SPELLLEVEL1);
						punching=3;
						punchingTimer=GetTickCount();
					}
				}
			}
		}
		else
		{
			if(next == BREAK)
				index+=2;
		}
		if(!mouse.L)
		{
			clickedDirection = INVALID;
		}
	}

	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	SetTexture1(game.Textures[GESTURES]);

	glColor4f(1,1,1,1);
	glPushMatrix();
	glTranslatef(400,300,0);
	if(clickedDirection != TOP)
	{
		glPushMatrix();
			glTranslatef(0,200,0);

			offset = 0.0f;
			if(highlight==TOP || (gesturedifficulty==EASY && (next==TOP && (index || clickedDirection==highlight))))	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}

	if(clickedDirection != BOTTOM)
	{
		glPushMatrix();
			glTranslatef(0,-200,0);

			offset = 0.0f;
			if(highlight==BOTTOM || (gesturedifficulty==EASY && (next==BOTTOM && (index || clickedDirection==highlight))))	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}

	if(clickedDirection != LEFT)
	{
		glPushMatrix();
			glTranslatef(-200,0,0);

			offset = 0.0f;
			if(highlight==LEFT || (gesturedifficulty==EASY && (next==LEFT && (index || clickedDirection==highlight))))	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}

	if(clickedDirection != RIGHT)
	{
		glPushMatrix();
			glTranslatef(200,0,0);

			offset = 0.0f;
			if(highlight==RIGHT || (gesturedifficulty==EASY && (next==RIGHT && (index || clickedDirection==highlight))))	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}

	if(clickedDirection != TOPRIGHT)
	{
		glPushMatrix();
			glTranslatef(141,141,0);

			offset = 0.0f;
			if(highlight==TOPRIGHT || (gesturedifficulty==EASY && (next==TOPRIGHT && (index || clickedDirection==highlight))))	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}

	if(clickedDirection != TOPLEFT)
	{
		glPushMatrix();
			glTranslatef(-141,141,0);

			offset = 0.0f;
			if(highlight==TOPLEFT || (gesturedifficulty==EASY && (next==TOPLEFT && (index || clickedDirection==highlight))))	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}

	if(clickedDirection != BOTTOMLEFT)
	{
		glPushMatrix();
			glTranslatef(-141,-141,0);

			offset = 0.0f;
			if(highlight==BOTTOMLEFT || (gesturedifficulty==EASY && (next==BOTTOMLEFT && (index || clickedDirection==highlight))))	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}

	if(clickedDirection != BOTTOMRIGHT)
	{
		glPushMatrix();
			glTranslatef(141,-141,0);

			offset = 0.0f;
			if(highlight==BOTTOMRIGHT || (gesturedifficulty==EASY && (next==BOTTOMRIGHT && (index || clickedDirection==highlight))))	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}
	glPopMatrix();



// Displaying the spell's formation
	if(showingSpell!=-1 && showMenu && spell)
	{
		if(difficulty<=NORMAL)
		{
			glColor3f(1,0,0);
			if(spell==(Directions*)Fireball)
				game.Print(-1,0,0,"Make combos by punching inflamed zombies at others");
			else if(spell==(Directions*)RainSpell)
				game.Print(-1,0,0,"Cast rain to slow the enemies down/");
				game.Print(500,540,0,"Click to start");
			glColor3f(1,1,1);
		}
		Directions showhighlight;
		Directions shownext;

		showhighlight = spell[showingSpell];
		shownext	  = spell[showingSpell+1];
		if(shownext != INVALID && shownext != BREAK && GetTickCount()-lastTick>50)
		{
			Vector startLine = dirVectors[showhighlight];
			Vector endLine = dirVectors[shownext];

			Vector diff = endLine-startLine;
			float length = diff.length();

			if(length)
			{
				if(!(spell[showingSpell+2]==END && (long)(GetTickCount())-(long)(showSpellTimer)>0))
				{
					diff/=length;					// Normalize the vector

					long diffTimeMS=(long)(GetTickCount())-(long)(showSpellTimer);
					float diffTime=(float)(diffTimeMS)/500;
					if(spell[showingSpell+2]==END)
					{
						diffTime+=2.0f;
						if(diffTime>1)
							diffTime=1;
					}
					length*=diffTime;

					Vector oldVal = startLine+Vector(400,300,0);
					for(float a=0;a<length;a+=length/25)
					{
						clickStart = oldVal + diff*a;
						gestureTrail->ForceActivateParts(5);
					}
				}
			}
			lastTick=GetTickCount();
			if((long)(GetTickCount())-(long)(showSpellTimer)>500)
			{
				showSpellTimer=GetTickCount();

				showingSpell++;

				if(spell[showingSpell+2]==END)
					showSpellTimer+=1000;
			}
		}
		else if(shownext==BREAK)
			showingSpell+=2;
		else if(shownext==END)
			showingSpell=0;
	}
}

void GameLevel1Module::Shutdown()
{
	parts.ShutdownEngine();

	scoreText* iter=head;
	while(iter)
	{
		scoreText* next=iter->next;
		delete iter;
		iter=next;
	}

	bgmchannel->stop();
	bgmintrochannel->stop();
	bgm->release();
	bgmintro->release();

	level1Score.score=wiz.score;
}
