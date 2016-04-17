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

#include "GestureTrail.h"
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

int GestureTrail::RenderParticles(VECTOR_CLASS* rminu,VECTOR_CLASS* rpluu,float processParts)
{
	glDepthMask(GL_FALSE);
	int numRendered=0;
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	SetTexture1(game.Textures[PARTICLE]);
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
				particles[a].pos+=particles[a].vel*g_speed*processParts;
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

	glDepthMask(GL_TRUE);
	return numRendered;
}
void GestureTrail::ForceActivateParts(int num)
{
	LastTick=GetTickCount();
	for(int a=activatedID;a<activatedID+num;a++)
	{
		ResetPart(a%numParticles,1);
	}
	activatedID += num;
	activatedID %= numParticles;
}

void GestureTrail::ResetPart(int ID,int forced)
{
	if(!forced)
	{
		particles[ID].state=0;
		return;
	}
	particles[ID].StartTick=GetTickCount();
	particles[ID].state|=STATE_ON;
	particles[ID].r=rand()%(r+1);
	particles[ID].g=rand()%(g+1);
	particles[ID].b=rand()%(b+1);
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
		particles[ID].life=500;
	}

	particles[ID].pos=VECTOR_CLASS(start->x+sinV,start->y+cosV/*+sin((float)particles[ID].counter/10)*20*/,0);
	particles[ID].vel=VECTOR_CLASS((float)(rand()%256-128),(float)(rand()%256-128),0);
	particles[ID].vel.normalize();
	particles[ID].vel*=(float)(rand()%100)/25;
}

void GestureTrail::GiveData(void* data)
{
// Typecast our data type, much like windows does when passing large structures
	GestureTrailData* sData=(GestureTrailData*)data;
	size=sData->size;
	radius=sData->radius;
}

void GestureTrail::SetNumParts(int num)
{
	activatedID=0;
	size=10;
	radius=5;
	CLEAN(particles);
	RESIZE(particles,Particle,num,0);
	numParticles=num;
	ZeroMemory(particles,sizeof(Particle)*numParticles);
}

// Coded by Jeff Verkoeyen ©2004