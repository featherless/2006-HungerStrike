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

#include "GameLevel2Module.h"
#include "GameDefs.h"
#include "LoseModule.h"
#include "WinLevel2Module.h"
#include "MainMenuModule.h"
#include "SFXEngine.h"

#define LEVELLENGTH		(90)

enum Level2Stages { FADEIN, SPEAKING, ROCKFLYIN, DODGEATTACK, FADEBACKIN, SCREWTHIS, FLYUP, GAME };
enum Level2BossStages { NOTINBOSS, ROCKFADEOUT, BOSS, BOSSDEAD };

Module* GameLevel2Mod;

#define GESTURESIZE		32

const int sizes[] = { 16, 32, 64 };

#define BOSSFADEINTIME		5000

void GameLevel2Module::GiveData(void* data)
{}

const int rockDiffs[NUMDIFFLEVELS]={ 6, 12, 18, 24 };
const int bossLives[NUMDIFFLEVELS] = { 2, 4, 6, 9 };

void GameLevel2Module::Initialize()
{
	if(gameDat.level==0)
		gameDat.level=1;
	Fireball=(Directions*)FireballSpells[gesturedifficulty];
	RainSpell=(Directions*)RainSpellSpells[gesturedifficulty];
	numRocksDiff=rockDiffs[difficulty];
	parts.ShutdownEngine();

	FMOD_RESULT result;
    result = fmodSystem->createStream("music/Level 2.ogg", FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &bgm);
	if (result != FMOD_OK)
		MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error loading music/Level 2.ogg!",L"Error",MB_OK);

    result = fmodSystem->createStream("music/Level 2 Intro.ogg", FMOD_SOFTWARE | FMOD_LOOP_OFF | FMOD_2D, 0, &bgmintro);
	if (result != FMOD_OK)
		MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error loading music/Level 2 Intro.ogg!",L"Error",MB_OK);

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

	wiz.x = 100;
	wiz.y = 160;
	wiz.xv= 0;
	wiz.facing=1;
	wiz.hover=0;
	wiz.score=0;
	wiz.kills=0;
	wiz.injuredTick=0;
	levelOffset=mountainOffset=0;

	spell = (Directions*)Fireball;

// Level 2 objects
	int a;
	for(a=0;a<NUMLINES;a++)
	{
		lines[a].length=rand()%200+40;
		lines[a].x=800+lines[a].length;
		lines[a].y=rand()%600;
		lines[a].xv=(float)(rand()%1000+200)/100;
	}
	for(a=0;a<numRocksDiff;a++)
	{
		rocks[a].size=sizes[rand()%3];
		rocks[a].x=rand()%1600+800+rocks[a].size;
		rocks[a].y=rand()%400+100;
		rocks[a].rot=0;
		rocks[a].xv=(float)(rand()%2000+1000)/1000;
		rocks[a].on=1;
	}

	glLineWidth(1);



// Animation INIT
	punching=0;
	punchingTimer=GetTickCount();


	shot.explodeGround=false;

	lastTick = GetTickCount();

// Set to non-negative-one to start the spell showing
	showingSpell=0;
	showSpellTimer=GetTickCount();

	doGesture=0;
	showMenu=0;

	wiz.health=100;

	wiz.injuredTick=GetTickCount();
	punchingTimer=GetTickCount();

	head=curr=0;

	combo=0;

	result = fmodSystem->playSound(FMOD_CHANNEL_FREE, bgmintro, false, &bgmintrochannel);
	if (result != FMOD_OK)
		MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error playing music/Level 2 Intro.ogg!",L"Error",MB_OK);
	bgmchannel=false;

	stage=FADEIN;
	bossStage=NOTINBOSS;
	levelBeginTick=GetTickCount();
	rainSlowTick=0;

	introZombie.createRandom();
	introZombie.respawn=0;

	introRock.x=928;
	introRock.size=64;
	introRock.y=200;
	introRock.rot=0;

	boss.life=bossLives[difficulty];
	boss.x=672;
	boss.animation=0;
	boss.animTick=GetTickCount();
	boss.pathTick=GetTickCount();
	boss.y=-128;
	boss.dir=3;
	boss.pathCount=0;
	boss.attackType=0;

	menuOpenedTick=0;
	beatLevel=0;
}

Module* GameLevel2Module::ExecuteModule()
{
	bool isPlaying=false;
	posX=(int)((mouse.X-game.newPosX));
	posY=(int)((600-(mouse.Y-game.newPosY)));
	levelOffsetVect=Vector(levelOffset,0,0);

	if(keys[VK_ESCAPE] && !lkeys[VK_ESCAPE] && stage!=GAME)
	{
		stage=GAME;
		levelBeginTick=GetTickCount();

		shot.shooting=0;
		wiz.x=100;
		wiz.y=160;
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
		bgmintrochannel->getPosition(&songTime,FMOD_TIMEUNIT_MS);
	int timeDiff = LEVELLENGTH-(GetTickCount()-levelBeginTick)/1000;

	if(timeDiff<0 && bossStage==NOTINBOSS && stage==GAME)
	{
		bossStage++;
		bossStageTick=GetTickCount();
		wiz.health=100;
	}
	if(bossStage==ROCKFADEOUT)
	{
		if(GetTickCount()-bossStageTick>=BOSSFADEINTIME)
		{
			for(int a=0;a<numRocksDiff;a++)
				rocks[a].on=false;
			bossStage++;
		}

		game.Print(-1,64,0,"You'll never see the boss if I can help it/");
	}
	else if(bossStage==BOSS)
	{
		if(!showMenu)
		{
			if(boss.attackType!=2)
				boss.y+=boss.dir*g_speed;
			else
				boss.y+=boss.dir*g_speed*5;

			if(boss.y+128>600 && boss.dir>0 || boss.y-128<0 && boss.dir<0)
			{
				if(boss.attackType==1)	boss.dir=0;
				else					boss.dir=-boss.dir;
			}

			int diff;
			if(boss.attackType==0)
			{
				if(boss.pathCount>0)	diff=1000;
				else					diff=1000;
			}
			else if(boss.attackType==1)
			{
				if(boss.pathCount>0)	diff=300;
				else					diff=1000;
			}
			else if(boss.attackType==2)
				diff=2000;
			if(boss.attackType==0 && GetTickCount()-boss.pathTick>diff && (boss.y>400 || boss.y<200 || boss.pathCount>0))
			{
				for(int y=0;y<3;y++)
				{
					int a;
					for(a=0;a<numRocksDiff;a++)
					{
						if(!rocks[a].on)
							break;
					}
					if(a!=numRocksDiff)
					{
						rocks[a].size=32;
						rocks[a].x=boss.x-128;
						rocks[a].y=boss.y-120+y*80;
						rocks[a].rot=(float)(rand()%360);
						rocks[a].xv=3.0f;
						rocks[a].on=2;
					}
				}
				sfx.PlaySFX(CANNON);
				boss.pathCount++;
				boss.pathCount%=2;
				if(boss.pathCount==0)
					boss.attackType++;
				boss.pathTick=GetTickCount();
			}
			else if(boss.attackType==1 && GetTickCount()-boss.pathTick>diff && (boss.dir==0 || boss.pathCount>0))
			{
				int a;
				for(a=0;a<numRocksDiff;a++)
				{
					if(!rocks[a].on)
						break;
				}
				if(a!=numRocksDiff)
				{
					rocks[a].size=64;
					rocks[a].x=boss.x-140;
					rocks[a].y=boss.y-48;
					rocks[a].rot=(float)(rand()%360);
					rocks[a].xv=3.0f;
					rocks[a].on=2;
				}
				sfx.PlaySFX(CANNON);
				boss.pathCount++;
				boss.pathCount%=5;
				if(boss.pathCount==0)
				{
					boss.attackType=0;
					if(boss.y<300)	boss.dir=3;
					else			boss.dir=-3;
				}
				boss.pathTick=GetTickCount();
			}
			else if(boss.attackType==2 && GetTickCount()-boss.pathTick>diff)
			{
				boss.pathCount=0;
				boss.attackType=0;
				boss.pathTick=GetTickCount()+1000;
			}
		}
		else
		{
			boss.pathTick+=menuOpenedTickDiff;
		}

		Vector shotPos=shot.getPos();
		Vector bossPos=Vector(boss.x,boss.y,0);

		float dist=VectDist(shotPos,bossPos);
		if(dist<100 && shot.isShooting() && boss.attackType!=2)
		{
			shot.explodeCircle=1;
			shot.explode();
			boss.attackType=2;
			if(!boss.dir)
			{
				if(boss.y<300)	boss.dir=3;
				else			boss.dir=-3;
			}
			boss.pathTick=GetTickCount();

			sfx.PlaySFX(LEVEL2BOSSHIT);
			boss.life--;
			if(boss.life<=0)
			{
				boss.attackType=3;
				bossStage=BOSSDEAD;
				bossStageTick=GetTickCount();
				sfx.PlaySFX(LEVEL2BOSSDEATH);
			}
		}
	}
	else if(bossStage==BOSSDEAD)
	{
		boss.y-=g_speed*3;
		boss.x-=g_speed*3;
		if(boss.y+128<0)
		{
			beatLevel=1;
		}
	}

	if(stage==GAME)
	{
		glColor4f(0.5f,0.5f,0.5f,1);
		glPushMatrix();
		glTranslatef(0,0,-5);
		glDisable(GL_TEXTURE_2D);

		glBegin(GL_LINES);
		for(int a=0;a<NUMLINES;a++)
		{
			if(!showMenu)
			{
				lines[a].x-=lines[a].xv;

				if(lines[a].x+lines[a].length<0)
				{
					lines[a].length=rand()%200+40;
					lines[a].x=800+lines[a].length;
					lines[a].y=rand()%600;
					lines[a].xv=(float)(rand()%1000+200)/100;
				}
			}

			glVertex2f(lines[a].x-lines[a].length,lines[a].y);
			glVertex2f(lines[a].x+lines[a].length,lines[a].y);
		}
		glEnd();

		glEnable(GL_TEXTURE_2D);
		glPopMatrix();
	}

	if(stage==GAME)
	{
		input();
		coldet();
		render();
	}

	float scale=1;
	glColor4f(1,1,1,1);
	if(stage==FADEIN)
	{
		scale=((float)songTime)/2000;
		glColor4f(scale,scale,scale,scale);
		if(scale>=1.0f)
		{
			stage++;
			scale=1;
			glColor4f(1,1,1,1);
		}
		wiz.x=100*scale;
		render();
		introZombie.process(1);
		introZombie.x=600;
		introZombie.render(scale);
	}

	float levelYOffset=0;
	if(stage==FLYUP)
	{
		float timeDiff=((float)songTime-32000)/4000;
		levelYOffset=timeDiff*-500;
		render();
	}

	if(stage<GAME)
	{
		glEnable(GL_BLEND);
	// Tiled ground
		SetTexture1(game.Textures[GRASS]);
		glTranslatef(0,(int)levelYOffset,0);
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
	}

	if(stage==SPEAKING)
	{
		render();
		introZombie.process(1);
		introZombie.x=600;
		introZombie.render(scale);
		if(songTime<4500)
			game.Print(-1,400,0,"Where's my sandwich you one-legged freak?");
		else if(songTime<=9000)
		{
			game.Print(-1,464,0,"We never touched your sandwich/");
			if(songTime>=6000)
			{
				game.Print(-1,432,0,"We were just having a town meeting and you started");
				game.Print(-1,400,0,"beating on us/");
			}
		}
		else if(songTime<15000)
		{
			game.Print(-1,432,0,"You liar, that sandwich thief ran right in to your");
			game.Print(-1,400,0,"measly little town//");
		}
		else if(songTime<19000)
		{
			game.Print(-1,464,0,"I swear/  Please don't kill any more of us, we've");
			game.Print(-1,432,0,"been living peacefully with humans for hundreds");
			game.Print(-1,400,0,"of years//");
		}
		else if(songTime<19500)
			game.Print(-1,464,0,"Huh??");
		else
			stage++;
	}
	else if(stage==ROCKFLYIN)
	{
		float timeDiff=((float)songTime-19500)/1500;
		if(timeDiff>=1)
		{
			timeDiff=1;
			stage++;
			introZombie.killFire();
		}
		if(timeDiff>=0.2f && !introZombie.isOnFire() && introZombie.isAlive())
		{
			introZombie.ignite();
		}
		render();
		introZombie.process(1);
		introZombie.x=600;
		introZombie.render(scale);

		introRock.x=864-600*timeDiff;

		SetTexture1(game.Textures[ROCK]);
		glPushMatrix();
			introRock.rot+=g_speed*5;
			glTranslatef(introRock.x,introRock.y,0);
			glRotatef(introRock.rot,0,0,1);
			glBegin(GL_QUADS);
				glTexCoord2f(1,0.0f); glVertex2f( introRock.size, introRock.size);
				glTexCoord2f(0,0.0f); glVertex2f(-introRock.size, introRock.size);
				glTexCoord2f(0,1.0f); glVertex2f(-introRock.size,-introRock.size);
				glTexCoord2f(1,1.0f); glVertex2f( introRock.size,-introRock.size);
			glEnd();
		glPopMatrix();
	}
	else if(stage==DODGEATTACK)
	{
		float timeDiff=((float)songTime-21000)/5000;
		if(timeDiff>=1)
		{
			stage++;
			timeDiff=1;
		}
		glColor4f(1,1,1,1-timeDiff);
		wiz.x=100+timeDiff*60;
		wiz.hover=0;
		glDepthMask(GL_FALSE);
		render();
		wiz.x=100-timeDiff*60;
		wiz.hover=0;
		render();
		glDepthMask(GL_TRUE);
		glColor4f(1,1,1,1);
		introZombie.process(1);
		introZombie.x=600;
		introZombie.render(scale);

		introRock.x=264-150*timeDiff;

		SetTexture1(game.Textures[ROCK]);
		glPushMatrix();
			introRock.rot+=g_speed;
			glTranslatef(introRock.x,introRock.y,0);
			glRotatef(introRock.rot,0,0,1);
			glBegin(GL_QUADS);
				glTexCoord2f(1,0.0f); glVertex2f( introRock.size, introRock.size);
				glTexCoord2f(0,0.0f); glVertex2f(-introRock.size, introRock.size);
				glTexCoord2f(0,1.0f); glVertex2f(-introRock.size,-introRock.size);
				glTexCoord2f(1,1.0f); glVertex2f( introRock.size,-introRock.size);
			glEnd();
		glPopMatrix();
	}
	else if(stage==FADEBACKIN)
	{
		float timeDiff=((float)songTime-26000)/5000;
		if(timeDiff>=1)
		{
			stage++;
			timeDiff=1;
		}
		glColor4f(1,1,1,timeDiff);
		wiz.x=160-timeDiff*60;
		wiz.hover=0;
		glDepthMask(GL_FALSE);
		render();
		wiz.x=40+timeDiff*60;
		wiz.hover=0;
		render();
		glDepthMask(GL_TRUE);
		glColor4f(1,1,1,1);

		introRock.x=114-170*timeDiff;

		SetTexture1(game.Textures[ROCK]);
		glPushMatrix();
			introRock.rot+=g_speed;
			glTranslatef(introRock.x,introRock.y,0);
			glRotatef(introRock.rot,0,0,1);
			glBegin(GL_QUADS);
				glTexCoord2f(1,0.0f); glVertex2f( introRock.size, introRock.size);
				glTexCoord2f(0,0.0f); glVertex2f(-introRock.size, introRock.size);
				glTexCoord2f(0,1.0f); glVertex2f(-introRock.size,-introRock.size);
				glTexCoord2f(1,1.0f); glVertex2f( introRock.size,-introRock.size);
			glEnd();
		glPopMatrix();
	}
	else if(stage==SCREWTHIS)
	{
		if(songTime>=32000)
		{
			stage++;
			levelBeginTick=GetTickCount();
		}
		wiz.x=100;
		render();
		game.Print(-1,400,0,"Screw this/");
	}

	if(stage==GAME)
	{
		render2();

		update();
		glColor4f(1,1,1,1);
	}

	glDisable(GL_BLEND);
// Text and stuff
	//game.Print(0,16,0,"%f",fps);

	if(difficulty<=NORMAL && stage==GAME && timeDiff>LEVELLENGTH-10)
	{
		glColor3f(1,1,1);
		game.Print(-1,32,0,"Blast through the rocks/");
		game.Print(-1,16,0,"Press W and S to move");
		game.Print(-1,0,0,"Press space to do a gesture");
	}
	shot.render(!showMenu);

	parts.RenderIndex(gestureTrailID,1);

	parts.RenderAll(!showMenu);

	if(wiz.health<=10 && stage==GAME)	return LoseMod;
	//if(timeDiff<=0 && stage==GAME)		return WinLevel1Mod;

	menuOpenedTickDiff=GetTickCount()-menuOpenedTick;
	menuOpenedTick=GetTickCount();

	if(beatLevel)		return WinLevel2Mod;
	return GameLevel2Mod;
}

void GameLevel2Module::input()
{
// Movement and punching
	if(!showMenu)
	{
		if(keys['S']!=keys['W'])
		{
			if(keys['W'])
			{
				wiz.y+=g_speed*8;
				if(wiz.y>472)
					wiz.y=472;
			}
			if(keys['S'])
			{
				wiz.y-=g_speed*8;
				if(wiz.y<128)
					wiz.y=128;
			}
		}
	}

// Bringing up the spell menu
	if(!shot.isShooting() && keys[VK_SPACE] && !lkeys[VK_SPACE] && bossStage!=ROCKFADEOUT)
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

void GameLevel2Module::coldet()
{
	int timeDiff = LEVELLENGTH-(GetTickCount()-levelBeginTick)/1000;
}

void GameLevel2Module::render()
{
	int timeDiff = LEVELLENGTH-(GetTickCount()-levelBeginTick)/1000;
	if(timeDiff<0)
		timeDiff=0;
// Rendering the gestures
	if(doGesture)	renderGestures();

// Color the background if the menu's displayed
	if(stage==GAME)
	{
		if(showMenu)	glColor4f(0.5f,0.5f,0.5f,1);
		else			glColor4f(1,1,1,1);
	}

	if(stage==GAME)
	{
		if(bossStage==ROCKFADEOUT)
		{
			float bossDiff=(float)(GetTickCount()-bossStageTick)/BOSSFADEINTIME;
			glColor4f(1,1,1,1-bossDiff);
			boss.y=-128+428*bossDiff;
		}
		glEnable(GL_BLEND);
		SetTexture1(game.Textures[ROCK]);
		for(int a=0;a<numRocksDiff*((float)(LEVELLENGTH-timeDiff)/LEVELLENGTH);a++)
		{
			if(!rocks[a].on)
				continue;
			if(!showMenu)
			{
				if(rocks[a].x+rocks[a].size<0)
				{
					if(rocks[a].on==1)
					{
						rocks[a].size=sizes[rand()%3];
						rocks[a].x=rand()%1600+800+rocks[a].size;
						rocks[a].y=rand()%400+100;
						rocks[a].rot=0;
						rocks[a].xv=(float)(rand()%2000+1000)/1000;
					}
					else
						rocks[a].on=0;
				}
				rocks[a].x-=rocks[a].xv*g_speed*2;
				rocks[a].rot+=g_speed*2;

				if(shot.isShooting() && bossStage!=ROCKFADEOUT)
				{
					Vector shotPos=shot.getPos();
					Vector rockPos=Vector(rocks[a].x,rocks[a].y,0);
					float dist=VectDist(shotPos,rockPos);
					if(dist<=rocks[a].size+32)
					{
						shot.explodeCircle=1;
						shot.explode();

						if(rocks[a].on==1)
						{
							rocks[a].size=sizes[rand()%3];
							rocks[a].x=rand()%1600+800+rocks[a].size;
							rocks[a].y=rand()%600;
							rocks[a].rot=0;
							rocks[a].xv=(float)(rand()%2000+1000)/1000;
						}
						else
							rocks[a].on=0;

						wiz.score+=25;
					}
				}

				if(rocks[a].y-rocks[a].size/2<=wiz.y+100 && rocks[a].y+rocks[a].size/2>=wiz.y-100 &&
					rocks[a].x-rocks[a].size<=wiz.x+10 && rocks[a].x+rocks[a].size>wiz.x-30 && bossStage!=ROCKFADEOUT)
				{
					if(rocks[a].size>=32 && wiz.injuredTick<=GetTickCount())
					{
						wiz.health-=20;
						wiz.injuredTick=GetTickCount()+1000;

						sfx.PlaySFX(MAINCHARHIT);
					}
					if(rocks[a].on==1)
					{
						rocks[a].size=sizes[rand()%3];
						rocks[a].x=rand()%1600+800+rocks[a].size;
						rocks[a].y=rand()%400+100;
						rocks[a].rot=0;
						rocks[a].xv=(float)(rand()%2000+500)/1000;
					}
					else
						rocks[a].on=0;
				}
			}
			glPushMatrix();
				glTranslatef(rocks[a].x,rocks[a].y,-4);
				glRotatef(rocks[a].rot,0,0,1);
				glBegin(GL_QUADS);
					glTexCoord2f(1,0.0f); glVertex2f( rocks[a].size, rocks[a].size);
					glTexCoord2f(0,0.0f); glVertex2f(-rocks[a].size, rocks[a].size);
					glTexCoord2f(0,1.0f); glVertex2f(-rocks[a].size,-rocks[a].size);
					glTexCoord2f(1,1.0f); glVertex2f( rocks[a].size,-rocks[a].size);
				glEnd();
			glPopMatrix();
		}
	}

// Render the boss
	if(stage==GAME && bossStage>=ROCKFADEOUT)
	{
		glLoadIdentity();
		SetTexture1(game.Textures[BOSS1]);
		glPushMatrix();
			glTranslatef(boss.x,boss.y,-2);

		glEnable(GL_BLEND);
// Render the boss
			float tx,ty;

			if(showMenu)
				boss.animTick+=menuOpenedTickDiff;

			if(GetTickCount()-boss.animTick>200)
			{
				boss.animation++;
				boss.animation%=3;
				boss.animTick=GetTickCount();
			}

			tx=(float)(boss.animation%2)*0.5f;
			ty=(float)((int)(boss.animation/2))*0.5f;

			glBegin(GL_QUADS);
			for(float x=0;x>-32;x-=4)
			{
				glColor4f(1,1,1,1+x/32);
				glTexCoord2f(tx+0.5f,ty); glVertex2f( 128+x, 128);
				glTexCoord2f(tx,	 ty); glVertex2f(-128+x, 128);
				glTexCoord2f(tx,	 ty+0.5f); glVertex2f(-128+x,-128);
				glTexCoord2f(tx+0.5f,ty+0.5f); glVertex2f( 128+x,-128);
			}
			glEnd();

			glColor4f(1,1,1,1);
			glBegin(GL_QUADS);
				glTexCoord2f(tx+0.5f,ty); glVertex2f( 128, 128);
				glTexCoord2f(tx,	 ty); glVertex2f(-128, 128);
				glTexCoord2f(tx,	 ty+0.5f); glVertex2f(-128,-128);
				glTexCoord2f(tx+0.5f,ty+0.5f); glVertex2f( 128,-128);
			glEnd();
		glPopMatrix();
	}

// Render the wizard
	glLoadIdentity();
	SetTexture1(game.Textures[WIZARD]);
	glPushMatrix();
		glTranslatef(wiz.x,wiz.y,3);

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
					if(spell==(Directions*)Fireball && rainSlowTick<GetTickCount() && boss.attackType!=2)
					{
						sfx.PlaySFX(FIRESPELL);
						if(wiz.facing)
						{
							shot.setVel(Vector(15,0,0));
							shot.setPos(Vector(wiz.x+50,wiz.y+30,0));
						}
						else
						{
							shot.setVel(Vector(-15,0,0));
							shot.setPos(Vector(wiz.x-50,wiz.y+30,0));
						}
						shot.shoot();
					}
					punching=4;
				}
				offset=0.75f;
			}
			else					punching=0;
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
		if(stage<GAME)
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
		else
		{
			glBegin(GL_QUADS);
			for(float x=0;x>-32;x-=4)
			{
				glColor4f(1,1,1,1+x/32);
				glTexCoord2f(wiz.facing*0.25f+offset,0.0f); glVertex2f( 64+x-!render*1000, 128);
				glTexCoord2f((1-wiz.facing)*0.25f+offset,0.0f); glVertex2f(-64+x-!render*1000, 128);
				glTexCoord2f((1-wiz.facing)*0.25f+offset,1.0f); glVertex2f(-64+x-!render*1000,-128);
				glTexCoord2f(wiz.facing*0.25f+offset,1.0f); glVertex2f( 64+x-!render*1000,-128);
			}
			glEnd();

			glColor4f(1,1,1,1);

			glBegin(GL_QUADS);
				glTexCoord2f(wiz.facing*0.25f+offset,0.0f); glVertex2f( 64-!render*1000, 128);
				glTexCoord2f((1-wiz.facing)*0.25f+offset,0.0f); glVertex2f(-64-!render*1000, 128);
				glTexCoord2f((1-wiz.facing)*0.25f+offset,1.0f); glVertex2f(-64-!render*1000,-128);
				glTexCoord2f(wiz.facing*0.25f+offset,1.0f); glVertex2f( 64-!render*1000,-128);
			glEnd();
		}
	glPopMatrix();
}

void GameLevel2Module::render2()
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
}

void GameLevel2Module::update()
{
	int timeDiff = LEVELLENGTH-(GetTickCount()-levelBeginTick)/1000;
// Update the game
	if(!showMenu)
	{
		levelOffset+=wiz.xv*g_speed;
		mountainOffset+=wiz.xv*g_speed/2;
		shot.offset(Vector(wiz.xv*g_speed,0,0));

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
	if(timeDiff>0)
		game.Print(0,504,0,"Time Left: %d",timeDiff);
}

void GameLevel2Module::renderGestures()
{
	Directions highlight = INVALID;
	Directions next = INVALID;
	float offset;

	if(showingSpell==-1 && !showMenu && spell)
	{
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

						sfx.PlaySFX(SPELLLEVEL2);
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
			if(highlight==TOP)	offset=0.5f;
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
			if(highlight==BOTTOM)	offset=0.5f;
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
			if(highlight==LEFT)	offset=0.5f;
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
			if(highlight==RIGHT)	offset=0.5f;
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
			if(highlight==TOPRIGHT)	offset=0.5f;
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
			if(highlight==TOPLEFT)	offset=0.5f;
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
			if(highlight==BOTTOMLEFT)	offset=0.5f;
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
			if(highlight==BOTTOMRIGHT)	offset=0.5f;
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

void GameLevel2Module::Shutdown()
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
}




/*
const int sizes[] = { 16, 32, 64 };
void GameLevel2Module::Initialize()
{
	wiz.x = 100;
	wiz.y = 160;
	wiz.xv= 0;
	wiz.facing=1;
	wiz.hover=0;
	wiz.score=0;
	wiz.kills=0;
	wiz.injuredTick=0;
	glEnable(GL_BLEND);

	int a;
	for(a=0;a<NUMLINES;a++)
	{
		lines[a].length=rand()%200+40;
		lines[a].x=800+lines[a].length;
		lines[a].y=rand()%600;
		lines[a].xv=(float)(rand()%1000+200)/30;
	}
	for(a=0;a<numRocksDiff;a++)
	{
		rocks[a].size=sizes[rand()%3];
		rocks[a].x=rand()%1600+800+rocks[a].size;
		rocks[a].y=rand()%600;
		rocks[a].rot=0;
		rocks[a].xv=(float)(rand()%2000)/1000;
	}

	glLineWidth(2);
}

Module* GameLevel2Module::ExecuteModule()
{
	if(keys['W'])
	{
		wiz.y+=g_speed*8;
		if(wiz.y>472)
			wiz.y=472;
	}
	if(keys['S'])
	{
		wiz.y-=g_speed*8;
		if(wiz.y<128)
			wiz.y=128;
	}

// Render the wizard
	glLoadIdentity();
	SetTexture1(game.Textures[WIZARD]);
	glPushMatrix();
		glTranslatef(wiz.x,(int)wiz.y,-1);

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
			glBegin(GL_QUADS);
			for(float x=0;x>-32;x--)
			{
				glColor4f(1,1,1,1+x/32);
				glTexCoord2f(wiz.facing*0.25f,0.0f); glVertex2f( 64+x, 128);
				glTexCoord2f((1-wiz.facing)*0.25f,0.0f); glVertex2f(-64+x, 128);
				glTexCoord2f((1-wiz.facing)*0.25f,1.0f); glVertex2f(-64+x,-128);
				glTexCoord2f(wiz.facing*0.25f,1.0f); glVertex2f( 64+x,-128);
			}
			glEnd();

			glColor4f(1,1,1,1);

			glTranslatef(0,0,1);
			glBegin(GL_QUADS);
				glTexCoord2f(wiz.facing*0.25f,0.0f); glVertex2f( 64, 128);
				glTexCoord2f((1-wiz.facing)*0.25f,0.0f); glVertex2f(-64, 128);
				glTexCoord2f((1-wiz.facing)*0.25f,1.0f); glVertex2f(-64,-128);
				glTexCoord2f(wiz.facing*0.25f,1.0f); glVertex2f( 64,-128);
			glEnd();

		}
	glPopMatrix();

	SetTexture1(game.Textures[ROCK]);
		for(int a=0;a<numRocksDiff;a++)
		{
			if(rocks[a].x+rocks[a].size<0)
			{
				rocks[a].size=sizes[rand()%3];
				rocks[a].x=rand()%1600+800+rocks[a].size;
				rocks[a].y=rand()%600;
				rocks[a].rot=0;
				rocks[a].xv=(float)(rand()%2000)/2000;
			}
			glPushMatrix();
				rocks[a].x-=rocks[a].xv;
				rocks[a].rot+=g_speed/5;
				glTranslatef(rocks[a].x,rocks[a].y,0);
				glRotatef(rocks[a].rot,0,0,1);
				glBegin(GL_QUADS);
					glTexCoord2f(1,0.0f); glVertex2f( rocks[a].size, rocks[a].size);
					glTexCoord2f(0,0.0f); glVertex2f(-rocks[a].size, rocks[a].size);
					glTexCoord2f(0,1.0f); glVertex2f(-rocks[a].size,-rocks[a].size);
					glTexCoord2f(1,1.0f); glVertex2f( rocks[a].size,-rocks[a].size);
				glEnd();
			glPopMatrix();
		}

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);
	for(int a=0;a<NUMLINES;a++)
	{
		lines[a].x-=lines[a].xv;

		if(lines[a].x+lines[a].length<0)
		{
			lines[a].length=rand()%200+40;
			lines[a].x=800+lines[a].length;
			lines[a].y=rand()%600;
			lines[a].xv=(float)(rand()%1000+200)/30;
		}

		glVertex2f(lines[a].x-lines[a].length,lines[a].y);
		glVertex2f(lines[a].x+lines[a].length,lines[a].y);
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);
	return GameLevel2Mod;
}

*/