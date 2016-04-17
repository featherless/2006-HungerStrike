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

#ifndef EXPLOSION_H_
#define EXPLOSION_H_


//////////////////////////////////////////////////////////////////
///////////////////////// INCLUDES ///////////////////////////////
//////////////////////////////////////////////////////////////////
#include "ParticleSystem.h"




//////////////////////////////////////////////////////////////////
///////////////////////// STRUCTURES /////////////////////////////
//////////////////////////////////////////////////////////////////
struct ExplosionData
{
	float size;
	int life;
	int dir;		// 1 up, 2 left, 3 right, 4 all around
	int gibbs;

	ExplosionData() { gibbs=0; }
};


class Explosion : public ParticleEngineChild
{
private:
	int dir,life;
	float size;
	int gibbs;
	VECTOR_CLASS lastStart;
public:
	Explosion() { start = 0; gibbs=0; }
	int RenderParticles(VECTOR_CLASS* rminu,VECTOR_CLASS* rpluu,float processParts);
	void SetNumParts(int num);
	void ResetPart(int ID,int forced);
	void GiveData(void* data);	// Requires a ExplosionData* structure passed to it
};


#endif