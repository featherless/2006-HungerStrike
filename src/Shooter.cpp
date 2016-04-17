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

#include "Shooter.h"
#include "GameDefs.h"
#include "module.h"
#include "Burst.h"
#include "Explosion.h"
#include "SFXEngine.h"

void Shooter::update(float gameSpeed)
{
	if(gameSpeed && shooting)
	{
		vel.y -= g_speed/10*gameSpeed;
		pos += vel*g_speed*gameSpeed;
		if(pos.y<=50 && explodeGround || (pos.x>800 || pos.y<0) && !explodeGround)
		{
			explodePos=pos;

			shooting = 0;
			ParticleEngineChild* child = parts.getChild(partSystemID);
			child->GiveData(0);

			if(explodeGround)
			{
				sfx.PlaySFX(ZOMBIEEXPLODE);

				ExplosionData explosionData;
				explosionData.dir=1;
				explosionData.life=500;
				explosionData.size=16;
				explosionData.gibbs=true;

				child = parts.getChild(explodeSystemID);
				child->GiveData(&explosionData);
				child->ForceActivateParts(500);
			}
		}
	}
}

void Shooter::explode()
{
	explodePos=pos;

	shooting = 0;
	ParticleEngineChild* child = parts.getChild(partSystemID);
	child->GiveData(0);

	ExplosionData explosionData;
	if(explodeCircle)
		explosionData.dir=4;
	else
	{
		if(vel.x<0)		explosionData.dir=3;
		else if(vel.x>0)explosionData.dir=2;
		else			explosionData.dir=1;
	}
	explosionData.life=500;
	explosionData.size=16;
	explosionData.gibbs=0;

	child = parts.getChild(explodeSystemID);
	child->GiveData(&explosionData);
	child->ForceActivateParts(500);

	sfx.PlaySFX(ZOMBIEEXPLODE);
}

void Shooter::offset(Vector off)
{
	 pos+=off;
	 explodePos+=off;
}

void Shooter::setPosNoKill(Vector p)
{
	pos = p;
	ParticleEngineChild* child = parts.getChild(partSystemID);
	child->SetStartVect(&pos);

	child = parts.getChild(explodeSystemID);
	child->SetStartVect(&explodePos);
}

void Shooter::setPos(Vector p)
{
	pos = p;
	ParticleEngineChild* child = parts.getChild(partSystemID);
	child->GiveData(0);
	child->SetStartVect(&pos);

	child = parts.getChild(explodeSystemID);
	child->SetStartVect(&explodePos);
}

void Shooter::render(float gameSpeed)
{
	if(shooting)
		parts.RenderIndex(partSystemID,gameSpeed);
	else
	{
		ParticleEngineChild* child = parts.getChild(partSystemID);
		child->GiveData(0);
	}
}

void Shooter::shoot()
{
	shooting=1;

	BurstData burstData;
	burstData.radius=10;
	burstData.life=100;
	burstData.size=16;
	ParticleEngineChild* child = parts.getChild(partSystemID);
	child->GiveData(&burstData);
	child->ForceActivateParts(500);
}