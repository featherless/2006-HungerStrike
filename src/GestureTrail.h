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

#ifndef GESTURETRAIL_H_
#define GESTURETRAIL_H_


// Programmed by Jeff Verkoeyen



//////////////////////////////////////////////////////////////////
///////////////////////// INCLUDES ///////////////////////////////
//////////////////////////////////////////////////////////////////
#include "ParticleSystem.h"




//////////////////////////////////////////////////////////////////
///////////////////////// STRUCTURES /////////////////////////////
//////////////////////////////////////////////////////////////////
struct GestureTrailData
{
	int radius;
	float size;
};



class GestureTrail : public ParticleEngineChild
{
private:
	int radius;
	float size;
	int activatedID;
public:
	int RenderParticles(VECTOR_CLASS* rminu,VECTOR_CLASS* rpluu,float processParts);
	void SetNumParts(int num);
	void ResetPart(int ID,int forced);
	void GiveData(void* data);	// Requires a GestureTrailData* structure passed to it
	void ForceActivateParts(int num);
};

#endif

// Coded by Jeff Verkoeyen ©2004