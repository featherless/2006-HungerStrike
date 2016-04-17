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

#include "Explosion.h"
#include "GameDefs.h"

// Programmed by Jeff Verkoeyen

#define STATE_ON		0x00000001
#define STATE_GIBB1		0x00000002
#define STATE_GIBB2		0x00000004
#define STATE_GIBB3		0x00000008
#define STATE_GIBB4		0x00000010

#ifndef NULL
#define NULL	0
#endif

#ifndef CLEAN
#define CLEAN(value) { if(value) { delete [] value; value=NULL; } }
#endif

#ifndef RESIZE
#define RESIZE(value,type,newSize,Original) { if(!Original) value=new type[(newSize)]; else value=(type*)realloc((void*)(value),sizeof(type)*(newSize)); }
#endif

int Explosion::RenderParticles(VECTOR_CLASS* rminu,VECTOR_CLASS* rpluu,float processParts)
{
	VECTOR_CLASS startDiff=Vector(0,0,0);
	if(start)
		 startDiff = *start-lastStart;
	glDepthMask(GL_FALSE);
	int numRendered=0;
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	if(!gibbs)		SetTexture1(game.Textures[FIREBALL]);
	else
		SetTexture1(game.Textures[GIBBS]);
	glBegin(GL_QUADS);
	VECTOR_CLASS dl,dr,tr,tl;
	VECTOR_CLASS currPos;
	unsigned long currTick=GetTickCount();
	float difference=0;
	for(int a=0;a<numParticles;a++)
	{
		if(particles[a].state&STATE_ON)
		{
			if(processParts)
			{
				difference=(float)(currTick-particles[a].StartTick);
				if(difference>=particles[a].life)
				{
					ResetPart(a,0);
					continue;
				}
				if(particles[a].life)
					difference/=particles[a].life;
				particles[a].a=(unsigned char)(255-(unsigned char)(difference*255));
				//particles[a].size=size*(1-difference);

				particles[a].counter+=1*g_speed;

				particles[a].pos+=particles[a].vel*g_speed*processParts;
			}
			else		// Account for the paused time
				particles[a].StartTick+=(currTick-LastTick);

			particles[a].pos+=startDiff;
			currPos=particles[a].pos;

			numRendered++;
			glColor4ub((unsigned char)(particles[a].r*fadeIn),
						(unsigned char)(particles[a].g*fadeIn),
						(unsigned char)(particles[a].b*fadeIn),
						(unsigned char)(particles[a].a*fadeIn));

			if(gibbs)
			{
				float tx,ty;
				int id;
				if(particles[a].state&STATE_GIBB1)
					id=0;
				else if(particles[a].state&STATE_GIBB2)
					id=1;
				else if(particles[a].state&STATE_GIBB3)
					id=2;
				else if(particles[a].state&STATE_GIBB4)
					id=3;

				tx=(float)(id%2)*0.5f;
				ty=(float)((int)(id/2))*0.5f;
					
				glTexCoord2f(tx+0.5f,ty);		glVertex2f( particles[a].pos.x+particles[a].size, particles[a].pos.y+particles[a].size);
				glTexCoord2f(tx,ty);			glVertex2f( particles[a].pos.x-particles[a].size, particles[a].pos.y+particles[a].size);
				glTexCoord2f(tx,ty+0.5f);		glVertex2f( particles[a].pos.x-particles[a].size, particles[a].pos.y-particles[a].size);
				glTexCoord2f(tx+0.5f,ty+0.5f);	glVertex2f( particles[a].pos.x+particles[a].size, particles[a].pos.y-particles[a].size);
			}
			else
			{
				glTexCoord2f(1,0);	glVertex2f( particles[a].pos.x+particles[a].size, particles[a].pos.y+particles[a].size);
				glTexCoord2f(0,0);	glVertex2f( particles[a].pos.x-particles[a].size, particles[a].pos.y+particles[a].size);
				glTexCoord2f(0,1);	glVertex2f( particles[a].pos.x-particles[a].size, particles[a].pos.y-particles[a].size);
				glTexCoord2f(1,1);	glVertex2f( particles[a].pos.x+particles[a].size, particles[a].pos.y-particles[a].size);
			}
		}
	}
	glEnd();
	LastTick=currTick;

	glDepthMask(GL_TRUE);

	if(start)	lastStart=*start;
	return numRendered;
}

void Explosion::ResetPart(int ID,int forced)
{
	if(!forced)
	{
		particles[ID].state = 0;
		return;
	}
	if(start)
		lastStart=*start;
	particles[ID].StartTick=GetTickCount();
	particles[ID].state|=STATE_ON;
	particles[ID].r=r;
	particles[ID].g=g;
	particles[ID].b=b;
	particles[ID].a=255;
	particles[ID].size=size;
	if(gibbs)
	{
		int randGibb=rand()%4;

		switch(randGibb)
		{
		case 0:
			particles[ID].state|=STATE_GIBB1;
			break;
		case 1:
			particles[ID].state|=STATE_GIBB2;
			break;
		case 2:
			particles[ID].state|=STATE_GIBB3;
			break;
		case 3:
			particles[ID].state|=STATE_GIBB4;
			break;
		}
	}
	float sinV=sinf((float)ID),cosV=cosf((float)ID),cosVV=sinf((float)ID);

	particles[ID].pos=VECTOR_CLASS(start->x+sinV,start->y+cosV/*+sin((float)particles[ID].counter/10)*20*/,0);

	switch(dir)
	{
	case 1:
		particles[ID].vel=VECTOR_CLASS((float)(rand()%256-128),(float)(rand()%128-16),0);
		particles[ID].vel.normalize();
		particles[ID].vel*=(float)(rand()%100)/30;
		break;
	case 2:
		particles[ID].vel=VECTOR_CLASS((float)(-rand()%128+16),(float)(rand()%256-128),0);
		particles[ID].vel.normalize();
		particles[ID].vel*=(float)(rand()%100)/30;
		break;
	case 3:
		particles[ID].vel=VECTOR_CLASS((float)(rand()%128-16),(float)(rand()%256-128),0);
		particles[ID].vel.normalize();
		particles[ID].vel*=(float)(rand()%100)/30;
		break;
	case 4:
		particles[ID].vel=VECTOR_CLASS((float)(rand()%256-128),(float)(rand()%256-128),0);
		particles[ID].vel.normalize();
		particles[ID].vel*=(float)(rand()%100)/30;
		break;
	}

	particles[ID].life=(abs((rand()%500-250))+life);
}

void Explosion::GiveData(void* data)
{
// Typecast our data type, much like windows does when passing large structures
	if(!data)
	{
		start=0;
		return;
	}
	ExplosionData* sData=(ExplosionData*)data;
	size=sData->size;
	life=sData->life;
	dir=sData->dir;
	gibbs=sData->gibbs;
}

void Explosion::SetNumParts(int num)
{
	size=10;
	life=2000;
	dir =4;
	CLEAN(particles);
	RESIZE(particles,Particle,num,0);
	numParticles=num;
	ZeroMemory(particles,sizeof(Particle)*numParticles);
}

// Coded by Jeff Verkoeyen ©2004