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

#ifndef ZOMBIE_H_
#define ZOMBIE_H_

#include <windows.h>
#include <cVector.h>

class Zombie
{
private:
	int onFire;
	unsigned long ignitionTime;
	unsigned long punchedBack;
	float punchX;
	int pushDir;
	int health;
	int fireID;
	Vector pos;
	float speed;
	unsigned long animTick;

	Vector oldPos[3];
	int oldPosID[3];
	int index;
public:
	Zombie() { fireID=-1; punchedBack=0; score=0; index=0; respawn=1; }

	int respawn;
	float facing;
	int animation;
	float x;
	unsigned int* score;
	int moving;

	void createRandom();
	unsigned int process(float animate);
	void render(float fade);
	void update(float diff) { x+=diff; oldPos[0].x+=diff; oldPos[1].x+=diff; oldPos[2].x+=diff; };
	void killFire();
	void punch();
	int isOnFire() { return (fireID!=-1); }
	int isAlive() { return !onFire; }
	int isPunched() { return (punchedBack>GetTickCount()); }

	void ignite();
};

#endif