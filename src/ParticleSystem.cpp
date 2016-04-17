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
#include <windows.h>
#include <gl\gl.h>

// Programmed by Jeff Verkoeyen




//////////////////////////////////////////////////////////////////
////////////////////////// MACROS ////////////////////////////////
//////////////////////////////////////////////////////////////////
#define NO_EMPTY_CHILD		-1

#ifndef CLEAN
#define CLEAN(value) { if(value) { delete [] value; value=NULL; } }
#endif

#ifndef RESIZE
#define RESIZE(value,type,newSize,Original) { if(!Original) value=new type[(newSize)]; else value=(type*)realloc((void*)(value),sizeof(type)*(newSize)); }
#endif


ParticleEngineServer::ParticleEngineServer()
{
	children=NULL;
	numChildren=0;
}


// Set up the rendering and calculate the view frustum only once
void ParticleEngineServer::InitRendering()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	//glDepthMask(GL_FALSE);

	float modelview[16];
// Grab the model view matrix
	glGetFloatv(GL_MODELVIEW_MATRIX,modelview);

// Calculate the edges
	VECTOR_CLASS right(modelview[0],modelview[4],modelview[8]);
	VECTOR_CLASS up(modelview[1],modelview[5],modelview[9]);
	rminu=right-up;
	rpluu=right+up;
}

// Clean everything up
void ParticleEngineServer::CloseRendering()
{
// Reset the rendering stuff
	//glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

int ParticleEngineServer::RenderAll(float animate)
{
	InitRendering();
	int numRendered=0;
	for(int a=0;a<numChildren;a++)
	{
		if(children[a] && !childrenRendered[a])		// Validate that the child exists
		{
			numRendered+=children[a]->RenderParticles(&rminu,&rpluu,animate);
			if(children[a]->killFlag)
				KillChild(a);
		}
		childrenRendered[a] = 0;
	}
	CloseRendering();
	return numRendered;
}

int ParticleEngineServer::RenderIndex(int ID,float animate)
{
	InitRendering();
	int numRendered=0;
	if(children[ID])
	{
		numRendered=children[ID]->RenderParticles(&rminu,&rpluu,animate);
		childrenRendered[ID]=1;
		if(children[ID]->killFlag)
			KillChild(ID);
	}
	CloseRendering();
	return numRendered;
}

// Finds the first empty child in the list
int ParticleEngineServer::findEmptyChild()
{
	for(int a=0;a<numChildren;a++)
	{
		if(!children[a])
			return a;
	}
	return NO_EMPTY_CHILD;
}

// Gives a new child to the server
void ParticleEngineServer::giveChild(ParticleEngineChild* child,int* id)
{
	int index=findEmptyChild();
	if(index==NO_EMPTY_CHILD)
	{
		RESIZE(childrenIDPtrs,int*,numChildren+1,numChildren);
		RESIZE(children,ParticleEngineChild*,numChildren+1,numChildren);
		RESIZE(childrenRendered,int,numChildren+1,numChildren);
		children[numChildren]=child;
		childrenIDPtrs[numChildren]=id;
		*id=numChildren;
		numChildren++;
	}
	else					// Found an empty spot, so let's fill it in
	{
		children[index]=child;
		childrenIDPtrs[index]=id;
		childrenRendered[index]=0;
		*id=index;
	}
}



void ParticleEngineServer::KillChild(int ID)
{
	if(ID<0) ID=0;
	else if(ID>=numChildren) ID=numChildren-1;
	*childrenIDPtrs[ID]=-1;
	CLEAN(children[ID]);
}



// Kills all of the children engines and resets everything
void ParticleEngineServer::ShutdownEngine()
{
	for(int a=0;a<numChildren;a++)
		KillChild(a);
	CLEAN(children);
	numChildren=0;
}

// Coded by Jeff Verkoeyen ©2004