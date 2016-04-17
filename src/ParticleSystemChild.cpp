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


//////////////////////////////////////////////////////////////////
///////////////////////// INCLUDES ///////////////////////////////
//////////////////////////////////////////////////////////////////
#include "ParticleSystem.h"

// Programmed by Jeff Verkoeyen


//////////////////////////////////////////////////////////////////
////////////////////////// MACROS ////////////////////////////////
//////////////////////////////////////////////////////////////////
#ifndef NULL
#define NULL	0
#endif
#pragma warning(disable : 4514)

// Initialize the particle engine's defaults
ParticleEngineChild::ParticleEngineChild()
{
	particles=NULL;
	numParticles=0;
	fadeIn=1.0f;
	killFlag=0;
}


// Standard functions used by all particle engines
void ParticleEngineChild::SetStartVect(VECTOR_CLASS* vect)
{ start=vect; }

void ParticleEngineChild::SetGravity(float grav)
{ gravity=grav; }

void ParticleEngineChild::SetColor(unsigned char R,unsigned char G,unsigned char B)
{ r=R; g=G; b=B; }

void ParticleEngineChild::SetFlag(unsigned int flag)
{
	for(int a=0;a<numParticles;a++)
		particles[a].state|=flag;
}
void ParticleEngineChild::ToggleFlag(unsigned int flag)
{
	for(int a=0;a<numParticles;a++)
		particles[a].state^=flag;
}
void ParticleEngineChild::ForceActivateParts(int num)
{
	LastTick=GetTickCount();
	if(num>numParticles)	num=numParticles;
	for(int a=0;a<num;a++)	ResetPart(a,1);
}


// Coded by Jeff Verkoeyen ©2004