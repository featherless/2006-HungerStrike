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

#include "Zombie.h"
#include "GameDefs.h"
#include "Fire.h"
#include "Module.h"
#include "SFXEngine.h"
#include "Explosion.h"

#define IGNITEFOR		1000

void Zombie::render(float fade)
{
	if(health>0 && !onFire)
	{
		glPushMatrix();
			glLoadIdentity();
			glTranslatef(x,128,2);
			SetTexture1(game.Textures[ZOMBIE]);

			float offset=0.25f*(float)animation;

			glBegin(GL_QUADS);
				glTexCoord2f((1-facing)*0.25f+offset,0.0f); glVertex2f( 64, 128);
				glTexCoord2f(facing*0.25f+offset,0.0f); glVertex2f(-64, 128);
				glTexCoord2f(facing*0.25f+offset,1.0f); glVertex2f(-64,-128);
				glTexCoord2f((1-facing)*0.25f+offset,1.0f); glVertex2f( 64,-128);
			glEnd();
		glPopMatrix();
	}
}

void Zombie::ignite()
{
	onFire=true;

	FireData data;
	data.radius=48;
	data.size=8;
	data.life=1000;
	ParticleEngineChild* fire=new Fire;
	fire->SetNumParts(1000);
	fire->SetColor(255,0,0);
	fire->GiveData(&data);
	fire->SetStartVect(&pos);
	fire->ForceActivateParts(5000);
	parts.giveChild(fire,&fireID);


	oldPos[index]=Vector(x,100,0);
	ExplosionData explosionData;
	explosionData.dir=1;
	explosionData.life=1500;
	explosionData.size=16;
	explosionData.gibbs=true;

	ParticleEngineChild* explosion=new Explosion;
	explosion->SetNumParts(15);
	explosion->SetGravity(0.2f);
	explosion->SetColor(255,255,255);
	explosion->GiveData(&explosionData);
	explosion->SetStartVect(&oldPos[index]);
	explosion->ForceActivateParts(500);
	parts.giveChild(explosion,&oldPosID[index]);

	index++;
	index%=3;

	ignitionTime=GetTickCount();
	if(rand()%100<50)
		sfx.PlaySFX(rand()%10+ZOMBIE1);
}

void Zombie::killFire()
{
	if(fireID!=-1)
		parts.KillChild(fireID);
}

unsigned int Zombie::process(float animate)
{
	pos=Vector(x,0,0);
	if(!onFire && moving)
	{
		if(x<390)
		{
			facing=1;
			x+=g_speed*animate*speed;
		}
		else if(x>410)
		{
			facing=0;
			x-=g_speed*animate*speed;
		}
	}
	if(punchedBack>=GetTickCount())
	{
		float diff=1-(float)(punchedBack-GetTickCount())/100.0f;
		if(pushDir)		x=punchX-diff*80;
		else			x=punchX+diff*80;
	}

	if(GetTickCount()-animTick>100)
	{
		animTick=GetTickCount();
		animation++;
		animation%=4;
	}

	if(GetTickCount()-ignitionTime>=IGNITEFOR && onFire)
	{
		if(fireID!=-1)
			parts.KillChild(fireID);
		if(respawn)
			createRandom();

		if(score)
			*score+=50;
		return 50;
	}

	return 0;
}

void Zombie::punch()
{
	health--;

	punchX=x;

	if(health<=0)
	{
		if(fireID==-1 && !onFire)
		{
			ignitionTime=GetTickCount();
			sfx.PlaySFX(rand()%10+ZOMBIE1);

			ExplosionData explosionData;
			explosionData.dir=1;
			explosionData.life=1500;
			explosionData.size=16;
			explosionData.gibbs=true;

			oldPos[index]=Vector(x,100,0);
			ParticleEngineChild* explosion=new Explosion;
			explosion->SetNumParts(15);
			explosion->SetGravity(0.2f);
			explosion->SetColor(255,255,255);
			explosion->GiveData(&explosionData);
			oldPos[index]=Vector(x,100,0);
			explosion->SetStartVect(&oldPos[index]);
			explosion->ForceActivateParts(500);
			parts.giveChild(explosion,&oldPosID[index]);
		}
		onFire=true;
	}
	else
	{
		pushDir=facing;
		punchedBack=GetTickCount()+100;
		sfx.PlaySFX(PUNCHED);
	}
}

void Zombie::createRandom()
{
	moving=1;
	onFire=0;
	health=2;
	if(fireID>=0)
		parts.KillChild(fireID);
	fireID=-1;

	if(rand()%1000<500)
	{
		x=-80-rand()%100*16;
		facing=1;
	}
	else
	{
		x= 880+rand()%100*16;
		facing=0;
	}
	speed=(float)(rand()%3000+2000)/2500.0f;
	punchX=x;

	animTick=GetTickCount();
	animation=rand()%4;
}