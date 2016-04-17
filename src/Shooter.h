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

#ifndef SHOOTER_H_
#define SHOOTER_H_

#include <cVector.h>

class Shooter
{
private:
	Vector pos,explodePos;
	Vector vel;

	int partSystemID;
	int explodeSystemID;
public:
	int shooting;
	int explodeGround;
	int explodeCircle;
	Shooter() { shooting = 0; explodeGround=1; explodeCircle=0; }

	void setPos(Vector p);
	void setPosNoKill(Vector p);
	void setVel(Vector v) { vel = v; }

	void setPartID(int id) { partSystemID = id; }
	void setExplodeID(int id) { explodeSystemID = id; }

	Vector getPos() { return pos; }
	Vector getVel() { return vel; }
	int isShooting(){ return shooting; }
	void offset(Vector off);

	void explode();

	void update(float gameSpeed);

	void render(float gameSpeed);

	void shoot();
};

#endif