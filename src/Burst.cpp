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

#include "Burst.h"
#include "GameDefs.h"

// Programmed by Jeff Verkoeyen

#define STATE_ON		0x00000001

#ifndef NULL
#define NULL	0
#endif

#ifndef CLEAN
#define CLEAN(value) { if(value) { delete [] value; value=NULL; } }
#endif

#ifndef RESIZE
#define RESIZE(value,type,newSize,Original) { if(!Original) value=new type[(newSize)]; else value=(type*)realloc((void*)(value),sizeof(type)*(newSize)); }
#endif

int Burst::RenderParticles(VECTOR_CLASS* rminu,VECTOR_CLASS* rpluu,float processParts)
{
	int numRendered=0;
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	SetTexture1(game.Textures[FIREBALL]);
	glBegin(GL_QUADS);
	VECTOR_CLASS dl,dr,tr,tl;
	VECTOR_CLASS currPos;
	unsigned long currTick=GetTickCount();
	float difference=0;
	int suck=0;
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
				particles[a].size=size*(1-difference);

				particles[a].counter+=1*g_speed;
			}
			else		// Account for the paused time
				particles[a].StartTick+=(currTick-LastTick);

			currPos=particles[a].pos;

			numRendered++;
			glColor4ub((unsigned char)(particles[a].r*fadeIn),
						(unsigned char)(particles[a].g*fadeIn),
						(unsigned char)(particles[a].b*fadeIn),
						(unsigned char)(particles[a].a*fadeIn));

			glTexCoord2f(1,0);	glVertex2f( particles[a].pos.x+particles[a].size, particles[a].pos.y+particles[a].size);
			glTexCoord2f(0,0);	glVertex2f( particles[a].pos.x-particles[a].size, particles[a].pos.y+particles[a].size);
			glTexCoord2f(0,1);	glVertex2f( particles[a].pos.x-particles[a].size, particles[a].pos.y-particles[a].size);
			glTexCoord2f(1,1);	glVertex2f( particles[a].pos.x+particles[a].size, particles[a].pos.y-particles[a].size);
		}
	}
	glEnd();
	LastTick=currTick;

	return numRendered;
}

void Burst::ResetPart(int ID,int forced)
{
	if(done)
	{
		particles[ID].state=0;
		return;
	}
	particles[ID].StartTick=GetTickCount();
	particles[ID].state|=STATE_ON;
	particles[ID].r=r;
	particles[ID].g=g;
	particles[ID].b=b;
	particles[ID].a=255;
	particles[ID].size=size;
	float sinV=sinf((float)ID),cosV=cosf((float)ID),cosVV=sinf((float)ID);
	if(radius)
	{
		float dist=(float)(rand()%radius);
		float ratio=dist/(float)radius;
		sinV*=dist;
		cosV*=dist;
		cosVV*=dist;
		particles[ID].life=abs((rand()%250-125))+(int)(ratio*life);
	}

	particles[ID].pos=VECTOR_CLASS(start->x+sinV,start->y+cosV/*+sin((float)particles[ID].counter/10)*20*/,0);
	particles[ID].vel=VECTOR_CLASS(0,0,0);

	if(forced)	particles[ID].life = ID;
	else		particles[ID].life=(abs((rand()%500-250))+life);
}

void Burst::GiveData(void* data)
{
// Typecast our data type, much like windows does when passing large structures
	if(!data)
		done = 1;
	else
	{
		BurstData* sData=(BurstData*)data;
		size=sData->size;
		life=sData->life;
		radius=sData->radius;
		done = 0;
	}
}

void Burst::SetNumParts(int num)
{
	size=10;
	life=2000;
	radius=5;
	CLEAN(particles);
	RESIZE(particles,Particle,num,0);
	numParticles=num;
	ZeroMemory(particles,sizeof(Particle)*numParticles);
}

// Coded by Jeff Verkoeyen ©2004