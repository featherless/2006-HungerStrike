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

#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

//////////////////////////////////////////////////////////////////
///////////////////////// INCLUDES ///////////////////////////////
//////////////////////////////////////////////////////////////////
#include <cVector.h>		// Used for processing the vertices of the particles
#include "cFrustum.h"



//////////////////////////////////////////////////////////////////
//////////////////// EASY CUSTOMIZATIONS /////////////////////////
//////////////////////////////////////////////////////////////////
#define VECTOR_CLASS	Vector
// Change the above definition if your definition of a Vector
//  class does not have the same name, this way you won't have
//  to rewrite endless amounts of code....


//////////////////////////////////////////////////////////////////
///////////////////////// STRUCTURES /////////////////////////////
//////////////////////////////////////////////////////////////////
// Basic Particle structure
struct Particle
{
	VECTOR_CLASS pos,vel;			// Position and velocity
	unsigned char r,g,b,a;			// Use bytes to save space
	unsigned int life;				// How long will it live for?
	unsigned long StartTick;		// When did it come in to existence?
	float size;						// Size of the particle
	unsigned int state;				// State flags can be set here
	float counter;					// Misc counter, optional
};



//////////////////////////////////////////////////////////////////
/////////////////////////// CLASSES //////////////////////////////
//////////////////////////////////////////////////////////////////
class ParticleEngineChild
{
// These data types must be protected so that our children can
//  actually access this data without generating compiler errors
protected:

// Particle information
	Particle* particles;
	int numParticles;

// Variables that should be global for all engine systems
	VECTOR_CLASS* start;		// Where does the particle system start at?
	float gravity;			// How much gravity is there?

	unsigned char r,g,b;	// Default color
	unsigned long LastTick;
public:
	int killFlag;			// Set to true to kill this child on the next iteration
	float fadeIn;
	ParticleEngineChild();	// Constructs our data correctly

// Implemented functions from our children classes
	virtual int RenderParticles(VECTOR_CLASS* rminu,VECTOR_CLASS* rpluu,float processParts)=0;
	virtual void SetNumParts(int num)=0;
	virtual void ResetPart(int ID,int forced)=0;
	virtual void GiveData(void* data)=0;

// Optional implemented functions
	virtual void ForceActivateParts(int num);
	virtual void SetFlag(unsigned int flag);
	virtual void ToggleFlag(unsigned int flag);


// Non-implemented functions used for global variable types
	void SetStartVect(VECTOR_CLASS* vect);
	void SetGravity(float grav);
	void SetColor(unsigned char R,unsigned char G,unsigned char B);
};


// This is our particle engine server which processes all of our
//  particle engines very efficiently.
//
// Notes:
//
///////////////////////INITIALIZATION//////////////////////////////////
//  When using the particle engine server, you must create one
//   single instance of the server like so:
//
//   ParticleEngineServer partServer;
//
//  Then, you need to create a child engine pointer like so:
//
//   ParticleEngineChild* engineChild;
//
//  The entire server runs off of polymorphism so that you can
//   construct your own particle engine class to suit your needs
//   We now need to instantiate a version of the child:
//
//   engineChild=new YourCustomClass;
//
//  Where YourCustomClass is a class that inherits from ParticleEngineChild
//   and implements all of the functions.
//
//  Once you have created an instance of the class, you can customize
//   it and do whatever you need to be done.  Once the whole child has
//   been set up, you need to call the following line of code:
//
//  partServer.giveChild(engineChild);
//
//  This will give the child to the server and the server will automatically
//   enqueue the new child engine in to the list of engines.
//  This function also returns the ID in the queue that the child was placed
//   in so you can directly call it if you want to.
//
//
///////////////////////RENDERING///////////////////////////////////////
//  Now that we have given our child engine to the server, we can render
//   the children by doing this:
//
//  partServer.RenderAll(1);
//
//  If you pass a 0 to RenderAll, the children will be told that you want
//   them to still render, but not to process the animations, causing a sort
//   of "bullet time" effect so that you can still move, with the particle
//   engines frozen in state.
//
//
///////////////////////SHUTDOWN////////////////////////////////////////
//  At the end of our program, we need to make sure to close down our
//   engine by calling the following code:
//
//  partServer.ShutdownEngine();
//
//  That's it!  You should be able to use the particle engine server
//   efficiently now and create your own children engines.
//

class ParticleEngineServer
{
private:
// Our list of children
	ParticleEngineChild** children;
	int** childrenIDPtrs;
	int* childrenRendered;
	int numChildren;

// Hold our necessary particle information in memory
//  to create the billboarding effects
	VECTOR_CLASS rminu,rpluu;

// Internal functions that SHOULD NOT be called outside of this
//  class, used to efficienize the rendering process by
//  being called only once at the beginning.
	void InitRendering();
	void CloseRendering();

// Finds the first empty child in the list
	int findEmptyChild();
public:
	ParticleEngineServer();

// Give a pointer to a new child
	void giveChild(ParticleEngineChild* child,int* id);

// Renderers for our engine
	int RenderAll(float animate);
	int RenderIndex(int ID,float animate);

	ParticleEngineChild* getChild(int ID) { return children[ID]; }

	void KillChild(int ID);
	void ShutdownEngine();
};

// Texture function that loads our particles in to memory
void LoadParticleTextures();

#endif

// Coded by Jeff Verkoeyen ©2004