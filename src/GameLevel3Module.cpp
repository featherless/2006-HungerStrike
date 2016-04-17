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

#include "GameLevel3Module.h"
#include "GameDefs.h"
#include "LoseModule.h"
#include "WinLevel1Module.h"
#include "SFXEngine.h"
#include "MainMenuModule.h"
#include "GameWinModule.h"

enum Level1Stages { FADEIN, FLOATIN, THATSMYSAND, ANGRYFACE, DELAY, THROWTOMATO, DEFLECT, WIZSPEAK, GAME };

Directions* ProtectSpell;
const Directions* ProtectSpells[] = { ProtectEasy, ProtectNormal, ProtectHard, ProtectCrazy };

Module* GameLevel3Mod;

#define GESTURESIZE		32

const int tomatoSpeeds[NUMDIFFLEVELS] = { 2, 4, 5, 7 };
const int lifeDiffs[NUMDIFFLEVELS] = { 3, 4, 7, 9 };

void GameLevel3Module::GiveData(void* data)
{}

void GameLevel3Module::Initialize()
{
	gameDat.level=2;
	Fireball=(Directions*)FireballSpells[gesturedifficulty];
	ProtectSpell=(Directions*)ProtectSpells[gesturedifficulty];
	parts.ShutdownEngine();

	FMOD_RESULT result;
    result = fmodSystem->createStream("music/Level 3.ogg", FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &bgm);
	if (result != FMOD_OK)
		MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error loading music/Level 3.ogg!",L"Error",MB_OK);

    result = fmodSystem->createStream("music/Level 3 Intro.ogg", FMOD_SOFTWARE | FMOD_LOOP_OFF | FMOD_2D, 0, &bgmintro);
	if (result != FMOD_OK)
		MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error loading music/Level 3 Intro.ogg!",L"Error",MB_OK);

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

	wiz.x = 400;
	wiz.y = 300;
	wiz.xv= 0;
	wiz.facing=0;
	wiz.hover=0;
	wiz.score=0;
	wiz.kills=0;

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

	wiz.injuredTick=GetTickCount();
	punchingTimer=GetTickCount();


	result = fmodSystem->playSound(FMOD_CHANNEL_FREE, bgmintro, false, &bgmintrochannel);
	if (result != FMOD_OK)
		MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error playing music/Level 3 Intro.ogg!",L"Error",MB_OK);

	bgmchannel=false;

	stage=FADEIN;
	levelBeginTick=GetTickCount();

	boss.animTick=GetTickCount();
	boss.animation=3;
	boss.x=100;
	boss.y=128;
	boss.dir=3;
	boss.life=lifeDiffs[difficulty];

	pillar1Y=0;
	pillar2Y=0;

	tom.on=false;
	shieldTimer=0;
	shieldPulse=0;
	boss.pathCount=0;

	diedTick=GetTickCount();

	menuOpenedTick=GetTickCount();
}

Module* GameLevel3Module::ExecuteModule()
{
	bool isPlaying=false;
	posX=(int)((mouse.X-game.newPosX));
	posY=(int)((600-(mouse.Y-game.newPosY)));

	if(keys[VK_ESCAPE] && !lkeys[VK_ESCAPE] && stage!=GAME)
	{
		stage=GAME;
		levelBeginTick=GetTickCount();

		boss.pathTick=GetTickCount()+(rand()%6+1)*1000;

		shot.shooting=0;
		wiz.x=400;
		wiz.y=300;
		boss.y=140;
		pillar1Y=pillar2Y=0;
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
	/*	FMOD_RESULT result;
		result = fmodSystem->playSound(FMOD_CHANNEL_FREE, bgm, false, &bgmchannel);
		if (result != FMOD_OK)
			MessageBox(game.GLWindow.Settings.hwnd,"FMOD error playing music/Level 1.ogg!","Error",MB_OK);

		stage=GAME;
		levelBeginTick=GetTickCount();*/
	}
	else
	{
		bgmintrochannel->getPosition(&songTime,FMOD_TIMEUNIT_MS);
	}

	if(stage==GAME)
	{
		input();
		coldet();
		render();
	}

	if(stage==FLOATIN)
	{
		float diff=((float)songTime-1500)/4200;

		if(diff>1)
		{
			diff=1;
			stage++;
		}

		wiz.y=700-diff*400;

		render();
	}
	else if(stage==THATSMYSAND)
	{
		float diff=((float)songTime-5700)/10000;

		if(diff<0.5f)
			game.Print(-1,500,0,"My sandwich// There it is//");

		if(diff>0.25f && diff<0.9f)
			game.Print(-1,475,0,"I can't wait to eat it...It'll taste so good...");

		if(diff>=0.5f)
			game.Print(-1,500,0,"Oooh the toppings...so scrumptious...");

		if(diff>=0.9f)
			game.Print(-1,475,0,"It looks so fresh and delicious...");

		if(diff>0.4f && diff<=0.7f)
			game.Print(200,400,0,"Hey...");

		if(diff>0.7f && diff<=0.8f)
			game.Print(100,450,0,"Hello?");

		if(diff>0.8f)
			game.Print(150,450,0,"Hey///");

		if(diff>1)
		{
			diff=1;
			stage++;
		}
		render();
	}
	else if(stage==ANGRYFACE)
	{
		unsigned int length;
		bgmintro->getLength(&length,FMOD_TIMEUNIT_MS);
		float timeLeft=length-songTime;
		float timeOverall=length-15700;
		float diffTime=1-timeLeft/timeOverall;

		game.Print(200,400,0,"HEY");

		if(diffTime>0.32f)
			game.Print(100,432,0,"What about me//");

		bool p;
		bgmintrochannel->isPlaying(&p);
		if(!p)
		{
			diffTime=1;
			stage++;
			waitTick=GetTickCount();
		}
		render();
	}
	else if(stage==DELAY)
	{
		float diff=((float)GetTickCount()-waitTick)/1000;

		if(diff>1)
		{
			diff=1;
			stage++;
			waitTick=GetTickCount();
			tom.x=boss.x+32;
			tom.y=boss.y-60;
			tom.xv=3;
			tom.on=true;
			tom.rot=0;
			sfx.PlaySFX(EATTHIS);
		}
		render();
	}
	else if(stage==THROWTOMATO)
	{
		float diff=((float)GetTickCount()-waitTick)/1000;

		if(diff>1)
		{
			diff=1;
			stage++;
			waitTick=GetTickCount();
		}
		tom.x=boss.x+32+diff*200;

		glEnable(GL_BLEND);
		SetTexture1(game.Textures[TOMATO]);
		glLoadIdentity();
		glTranslatef((int)tom.x,(int)tom.y,2);
		tom.rot-=g_speed*3;
		glRotatef(tom.rot,0,0,1);

		glBegin(GL_QUADS);
			glTexCoord2f(1,0);	glVertex2f( 16, 16);
			glTexCoord2f(0,0);	glVertex2f(-16, 16);
			glTexCoord2f(0,1);	glVertex2f(-16,-16);
			glTexCoord2f(1,1);	glVertex2f( 16,-16);
		glEnd();

		render();

		if(diff>0.8f && shieldTimer<GetTickCount())
			shieldTimer=GetTickCount()+3000;
	}
	else if(stage==DEFLECT)
	{
		float diff=((float)GetTickCount()-waitTick)/3000;

		if(diff>1)
		{
			diff=1;
			stage++;
			waitTick=GetTickCount();
		}
		tom.x=boss.x+232-diff*800;
		tom.y=boss.y-60+diff*800;

		glEnable(GL_BLEND);
		SetTexture1(game.Textures[TOMATO]);
		glLoadIdentity();
		glTranslatef((int)tom.x,(int)tom.y,2);
		tom.rot-=g_speed*3;
		glRotatef(tom.rot,0,0,1);

		glBegin(GL_QUADS);
			glTexCoord2f(1,0);	glVertex2f( 16, 16);
			glTexCoord2f(0,0);	glVertex2f(-16, 16);
			glTexCoord2f(0,1);	glVertex2f(-16,-16);
			glTexCoord2f(1,1);	glVertex2f( 16,-16);
		glEnd();

		render();
	}
	else if(stage==WIZSPEAK)
	{
		float diff=((float)GetTickCount()-waitTick)/5000;

		game.Print(-1,540,0,"Hey, you have my sandwich/");

		if(diff>1)
		{
			diff=1;
			stage++;

			FMOD_RESULT result;
			result = fmodSystem->playSound(FMOD_CHANNEL_FREE, bgm, false, &bgmchannel);
			if (result != FMOD_OK)
				MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error playing music/Level 3.ogg!",L"Error",MB_OK);

			boss.pathTick=GetTickCount()+(rand()%3+1)*1000;

			stage=GAME;
			levelBeginTick=GetTickCount();
		}
		if(diff>0.2f)
		{
			game.Print(100,430,0,"....");
			float towerDiff=(diff-0.2f)/0.8f;
			pillar1Y=256-towerDiff*256;
			boss.y=256-towerDiff*256+140;
		}
		if(diff>0.5f)
		{
			game.Print(-1,516,0,"That's it, I'm going to beat you so hard");
			game.Print(-1,500,0,"I won't even have to move");
		}
		render();
	}

// Guitar fade in at 14600'ish
	float scale;
	if(stage==FADEIN)
	{
		scale=((float)songTime)/1500;

		pillar1Y=scale*256;
		boss.y=scale*256+140;

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






	int a;
	SetTexture1(game.Textures[PILLAR]);
	glLoadIdentity();

	glTranslatef(32,32,-4);
	glBegin(GL_QUADS);
	for(a=0;a<800;a+=64)
	{
		glTexCoord2f(1,0.75f);	glVertex2f( 32+a, 32);
		glTexCoord2f(0,0.75f);	glVertex2f(-32+a, 32);
		glTexCoord2f(0,1);		glVertex2f(-32+a,-32);
		glTexCoord2f(1,1);		glVertex2f( 32+a,-32);
	}
	glEnd();

	glLoadIdentity();
	glTranslatef(100,(int)(pillar1Y-256),-3);

	glBegin(GL_QUADS);
		glTexCoord2f(1,0.50f);	glVertex2f( 32, 32);
		glTexCoord2f(0,0.50f);	glVertex2f(-32, 32);
		glTexCoord2f(0,0.75f);	glVertex2f(-32,-32);
		glTexCoord2f(1,0.75f);	glVertex2f( 32,-32);

	for(a=64;a<64*4;a+=64)
	{
		glTexCoord2f(1,0.25f);	glVertex2f( 32, 32+a);
		glTexCoord2f(0,0.25f);	glVertex2f(-32, 32+a);
		glTexCoord2f(0,0.50f);	glVertex2f(-32,-32+a);
		glTexCoord2f(1,0.50f);	glVertex2f( 32,-32+a);
	}
		glTexCoord2f(1,0.00f);	glVertex2f( 32, 32+a);
		glTexCoord2f(0,0.00f);	glVertex2f(-32, 32+a);
		glTexCoord2f(0,0.25f);	glVertex2f(-32,-32+a);
		glTexCoord2f(1,0.25f);	glVertex2f( 32,-32+a);
	glEnd();

	glLoadIdentity();
	glTranslatef(700,(int)(pillar2Y-256),-3);
	glBegin(GL_QUADS);
		glTexCoord2f(1,0.50f);	glVertex2f( 32, 32);
		glTexCoord2f(0,0.50f);	glVertex2f(-32, 32);
		glTexCoord2f(0,0.75f);	glVertex2f(-32,-32);
		glTexCoord2f(1,0.75f);	glVertex2f( 32,-32);

	for(a=64;a<64*4;a+=64)
	{
		glTexCoord2f(1,0.25f);	glVertex2f( 32, 32+a);
		glTexCoord2f(0,0.25f);	glVertex2f(-32, 32+a);
		glTexCoord2f(0,0.50f);	glVertex2f(-32,-32+a);
		glTexCoord2f(1,0.50f);	glVertex2f( 32,-32+a);
	}
		glTexCoord2f(1,0.00f);	glVertex2f( 32, 32+a);
		glTexCoord2f(0,0.00f);	glVertex2f(-32, 32+a);
		glTexCoord2f(0,0.25f);	glVertex2f(-32,-32+a);
		glTexCoord2f(1,0.25f);	glVertex2f( 32,-32+a);
	glEnd();



	if(showMenu)
		boss.pathTick+=menuOpenedTickDiff;

	if(stage==GAME)
	{
		if(boss.pathCount==0 && !showMenu && boss.life>0)
		{
			boss.x+=g_speed*boss.dir;

			if(boss.x>700 && boss.pathTick<GetTickCount())
			{
				// Rise on platform 2!
				boss.pathCount=2;
				boss.pathTick=GetTickCount();
				boss.animation=3;
				boss.dir=-5;
				sfx.PlaySFX(PILLARSFX);
			}
			if(boss.x<100 && boss.pathTick<GetTickCount())
			{
				// Rise on platform 1!
				boss.pathCount=3;
				boss.pathTick=GetTickCount();
				boss.animation=3;
				boss.dir=5;
				sfx.PlaySFX(PILLARSFX);
			}
			if(boss.x+30>750 && boss.dir>0)		boss.dir=-boss.dir;
			else if(boss.x-30<50 && boss.dir<0)	boss.dir=-boss.dir;
			if(GetTickCount()-boss.animTick>100)
			{
				boss.animation++;
				boss.animation%=5;
				boss.animTick=GetTickCount();
			}
		}
// Rising on platform 1
		else if(boss.pathCount==3)
		{
			float diff=(float)(GetTickCount()-boss.pathTick)/500;
			if(diff>1)
			{
				diff=1;
				boss.pathCount+=2;
				tom.x=boss.x+32;
				tom.y=boss.y-60;
				tom.xv=tomatoSpeeds[difficulty];
				tom.yv=0;
				tom.on=true;
				tom.rot=0;
				sfx.PlaySFX(EATTHIS);
			}
			pillar1Y=diff*256;
			boss.y=diff*256+140;
		}
// Rising on platform 2
		else if(boss.pathCount==2)
		{
			float diff=(float)(GetTickCount()-boss.pathTick)/500;
			if(diff>1)
			{
				diff=1;
				boss.pathCount+=2;
				tom.x=boss.x-32;
				tom.y=boss.y-60;
				tom.xv=-tomatoSpeeds[difficulty];
				tom.yv=0;
				tom.on=true;
				tom.rot=0;
				sfx.PlaySFX(EATTHIS);
			}
			pillar2Y=diff*256;
			boss.y=diff*256+140;
		}
		else if(boss.pathCount==7)
		{
			float diff=(float)(GetTickCount()-boss.pathTick)/500;
			if(diff>1)
			{
				diff=1;
				boss.pathCount=0;
				boss.pathTick=GetTickCount()+(rand()%5+2)*1000;
			}
			pillar1Y=256-diff*256;
			boss.y=256-diff*256+140;
		}
		else if(boss.pathCount==6)
		{
			float diff=(float)(GetTickCount()-boss.pathTick)/500;
			if(diff>1)
			{
				diff=1;
				boss.pathCount=0;
				boss.pathTick=GetTickCount()+(rand()%6+1)*1000;
			}
			pillar2Y=256-diff*256;
			boss.y=256-diff*256+140;
		}
		else if(boss.pathCount==8 || boss.pathCount==9)
		{
			if(!showMenu)
			{
				tom.x+=tom.xv*g_speed;
				tom.rot-=g_speed*3;

				if(fabsf(tom.x-boss.x)<32)
				{
					ExplosionData explosionData;
					explosionData.dir=boss.pathCount-6;
					explosionData.life=1000;
					explosionData.size=8;
					explosionData.gibbs=false;

					ParticleEngineChild* explosion=new Explosion;
					explosion->SetNumParts(500);
					explosion->SetGravity(0.2f);
					explosion->SetColor(255,255,255);
					explosion->GiveData(&explosionData);
					Vector pos=Vector(tom.x,tom.y,0);
					explosion->SetStartVect(&pos);
					explosion->ForceActivateParts(500);
					explosion->GiveData(0);
					int id;
					parts.giveChild(explosion,&id);

					boss.pathCount+=2;
					boss.pathTick=GetTickCount();
				}
			}

			SetTexture1(game.Textures[TOMATO]);
			glLoadIdentity();
			glTranslatef((int)tom.x,(int)tom.y,2);
			glRotatef(tom.rot,0,0,1);

			glBegin(GL_QUADS);
				glTexCoord2f(1,0);	glVertex2f( 16, 16);
				glTexCoord2f(0,0);	glVertex2f(-16, 16);
				glTexCoord2f(0,1);	glVertex2f(-16,-16);
				glTexCoord2f(1,1);	glVertex2f( 16,-16);
			glEnd();
		}
		else if(boss.pathCount==10 | boss.pathCount==11)
		{
			float diff=(float)(GetTickCount()-boss.pathTick)/3000;
			if(diff>1)
			{
				diff=1;
				boss.pathCount-=4;
				boss.pathTick=GetTickCount();
			}
			Vector shotPos=shot.getPos();
			if(fabsf(shotPos.x-boss.x)<64)
			{
				shot.explodeCircle=boss.pathCount-8;
				shot.explode();
				boss.life--;
				diedTick=GetTickCount();
				if(boss.life)
					sfx.PlaySFX(FINALBOSSHIT);
				else
					sfx.PlaySFX(FINALBOSSDIE);

				boss.pathCount-=4;

				boss.pathTick=GetTickCount();
				sfx.PlaySFX(PILLARSFX);
			}
		}

		if(boss.pathCount==4 || boss.pathCount==5)
		{
			if(!showMenu)
			{
				tom.x+=tom.xv*g_speed;
				tom.rot-=g_speed*3;

				if(fabsf(tom.x-wiz.x)<32 && GetTickCount()>=shieldTimer)
				{
					ExplosionData explosionData;
					if(boss.pathCount==4)
						explosionData.dir=3;
					else
						explosionData.dir=2;
					explosionData.life=1000;
					explosionData.size=8;
					explosionData.gibbs=false;

					ParticleEngineChild* explosion=new Explosion;
					explosion->SetNumParts(500);
					explosion->SetGravity(0.2f);
					explosion->SetColor(255,255,255);
					explosion->GiveData(&explosionData);
					Vector pos=Vector(tom.x,tom.y,0);
					explosion->SetStartVect(&pos);
					explosion->ForceActivateParts(500);
					explosion->GiveData(0);
					int id;
					parts.giveChild(explosion,&id);

					wiz.injuredTick=GetTickCount()+1000;
					wiz.health-=25;
					boss.pathCount+=2;
					boss.pathTick=GetTickCount();

					sfx.PlaySFX(MAINCHARHIT);

					sfx.PlaySFX(PILLARSFX);
				}
				else if(fabsf(tom.x-wiz.x)<64 && GetTickCount()<shieldTimer)
				{
					tom.xv=-tom.xv;
					tom.xv*=2;
					boss.pathCount+=4;
				}
			}

			SetTexture1(game.Textures[TOMATO]);
			glLoadIdentity();
			glTranslatef((int)tom.x,(int)tom.y,2);
			glRotatef(tom.rot,0,0,1);

			glBegin(GL_QUADS);
				glTexCoord2f(1,0);	glVertex2f( 16, 16);
				glTexCoord2f(0,0);	glVertex2f(-16, 16);
				glTexCoord2f(0,1);	glVertex2f(-16,-16);
				glTexCoord2f(1,1);	glVertex2f( 16,-16);
			glEnd();
		}
	}


	if(boss.life<=0 && GetTickCount()-diedTick<=2000)
	{
		float diff=(float)(GetTickCount()-diedTick)/2000;
		if(diff>1)
			diff=1;
		glColor4f(1,1,1,1-diff);
	}
	else if(boss.life<=0 && GetTickCount()-diedTick>=2000)
		return GameWinMod;
	glLoadIdentity();

	glTranslatef((int)boss.x,(int)boss.y,3);
	SetTexture1(game.Textures[BOSS2]);

	
	float tx,ty;
	tx=(float)(boss.animation%4)*0.25f;
	ty=(float)((int)(boss.animation/4))*0.5f;

	float flip=0;
	if(boss.dir<0)
		flip=1;
	glBegin(GL_QUADS);
		glTexCoord2f(tx+0.25f*flip,			ty);		glVertex2f( 64, 128);
		glTexCoord2f(tx+0.25f*(1-flip),		ty);		glVertex2f(-64, 128);
		glTexCoord2f(tx+0.25f*(1-flip),		ty+0.5f);	glVertex2f(-64,-128);
		glTexCoord2f(tx+0.25f*flip,			ty+0.5f);	glVertex2f( 64,-128);
	glEnd();


	glLoadIdentity();






	if(stage==GAME)
	{
		render2();

		update();
		glColor4f(1,1,1,1);
	}

	glDisable(GL_BLEND);
// Text and stuff
	//game.Print(0,16,0,"%f",fps);

	if(boss.pathCount>=10)
		game.Print(-1,200,0,"He's weak, hit him with a fireball/");

	shot.render(!showMenu);

	parts.RenderIndex(gestureTrailID,1);

	parts.RenderAll(!showMenu);

	if(wiz.health<=10 && stage==GAME)	return LoseMod;
	//if(timeDiff<=0 && stage==GAME)		return WinLevel1Mod;

	menuOpenedTickDiff=GetTickCount()-menuOpenedTick;
	menuOpenedTick=GetTickCount();

	return GameLevel3Mod;
}

void GameLevel3Module::input()
{
// Movement and punching
	if(!showMenu)
	{
		if(keys['D'])
			wiz.facing = 1;
		if(keys['A'])
			wiz.facing = 0;
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

void GameLevel3Module::coldet()
{
}

void GameLevel3Module::render()
{
// Rendering the gestures
	if(doGesture)	renderGestures();

// Color the background if the menu's displayed
	if(showMenu)	glColor4f(0.5f,0.5f,0.5f,1);
	else			glColor4f(1,1,1,1);

// Render the wizard
	glLoadIdentity();
	SetTexture1(game.Textures[WIZARD]);
	glPushMatrix();
		glTranslatef((int)wiz.x,(int)wiz.y,-1);

		if(shieldTimer>GetTickCount())
		{
			glDisable(GL_TEXTURE_2D);

			float offset=20;
			if(wiz.facing)
				offset=-20;

			shieldPulse+=g_speed/8;
			glBegin(GL_LINE_LOOP);
			for(float x=0;x<3.14159f*2;x+=3.14159f/30)
				glVertex2f((80+sinf(shieldPulse)*10)*cosf(x)+offset,(150+sinf(shieldPulse)*10)*sinf(x));
			glEnd();

			glEnable(GL_TEXTURE_2D);
		}

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
					if(spell==(Directions*)Fireball)
					{
						sfx.PlaySFX(FIRESPELL);
						if(wiz.facing)
						{
							shot.setVel(Vector(10-wiz.xv,0,0));
							shot.setPos(Vector(wiz.x+50,wiz.y+30,0));
						}
						else
						{
							shot.setVel(Vector(-10-wiz.xv,0,0));
							shot.setPos(Vector(wiz.x-50,wiz.y+30,0));
						}
						if(boss.pathCount<10)
							sfx.PlaySFX(BOSSLAUGH1+rand()%2);
						shot.shoot();
					}
					else if(spell==(Directions*)ProtectSpell)
					{
						shieldTimer=GetTickCount()+3000;
						shieldPulse=0;
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
		if(render)
		{
			wiz.hover+=g_speed/10;
			glTranslatef(0,(int)(sinf(wiz.hover)*5),0);
			glBegin(GL_QUADS);
				glTexCoord2f(wiz.facing*0.25f+offset,0.0f); glVertex2f( 64, 128);
				glTexCoord2f((1-wiz.facing)*0.25f+offset,0.0f); glVertex2f(-64, 128);
				glTexCoord2f((1-wiz.facing)*0.25f+offset,1.0f); glVertex2f(-64,-128);
				glTexCoord2f(wiz.facing*0.25f+offset,1.0f); glVertex2f( 64,-128);
			glEnd();
		}
	glPopMatrix();
}

void GameLevel3Module::render2()
{
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

void GameLevel3Module::update()
{
// Update the game
	if(!showMenu)
	{
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

		SetTexture1(game.Textures[SHIELD]);
		glPushMatrix();
			glTranslatef(336,568,5);

			glColor3f(1,1,1);
			if(posX>=336-32 && posX<=336+32 &&
			posY>=536 && posY<=600)
			{
				if(spell!=(Directions*)ProtectSpell)
				{
					spell=(Directions*)ProtectSpell;

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
	glColor4f(1,1,1,1);
}

void GameLevel3Module::renderGestures()
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
			glColor3f(0,0,0);
			if(spell==(Directions*)Fireball)
				game.Print(-1,0,0,"Cast when he is weakened to hurt him/");
			else if(spell==(Directions*)ProtectSpell)
				game.Print(-1,0,0,"Deflect the tomatoes at the boss/");
			glColor3f(0,1,0);
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

void GameLevel3Module::Shutdown()
{
	parts.ShutdownEngine();

	bgmchannel->stop();
	bgmintrochannel->stop();
	bgm->release();
	bgmintro->release();

	level1Score.score=wiz.score;
}