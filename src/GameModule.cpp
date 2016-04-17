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

#include "GameModule.h"
#include "GameDefs.h"

extern bool done;

Module* GameMod;

void GameModule::GiveData(void* data)
{}

void GameModule::Initialize()
{
	parts.ShutdownEngine();

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	tower1.setNuSects(1);
	tower2.setNuSects(1);

	partPos2=Vector(-100,-100,0);

	burst=new Burst;
	burst->SetNumParts(100);
	burst->SetGravity(0.2f);
	burst->SetColor(255,0,0);
	parts.giveChild(burst,&partEngIDs[0]);

	explosion=new Explosion;
	explosion->SetNumParts(500);
	explosion->SetGravity(0.2f);
	explosion->SetColor(255,0,0);
	parts.giveChild(explosion,&partEngIDs[1]);

	shot.setPartID(partEngIDs[0]);
	shot.setExplodeID(partEngIDs[1]);

	clicking=0;

	glLineWidth(2);
}

Module* GameModule::ExecuteModule()
{
	posX=(int)((mouse.X-game.newPosX));
	posY=(int)((600-(mouse.Y-game.newPosY)));

	if(mouse.L && !mouse.hL && !hud.isDocking())
	{
		mouse.hL = true;

		clicking = true;

		clickStart = Vector((float)posX,(float)posY,0);
	}

	if(!mouse.L && clicking)
	{
		clicking = 0;

		Vector diff = Vector((float)posX,(float)posY,0) - clickStart;
		if(diff.length()>30)
		{
			diff.normalize();
			diff*=30;
		}
		diff/=4;
		shot.setPos(clickStart);
		shot.setVel(diff);
		shot.shoot();
	}

	if(clicking)
	{
		glDisable(GL_TEXTURE_2D);

		Vector diff = Vector((float)posX,(float)posY,0) - clickStart;
		Vector edge;
		if(diff.length()>30)
		{
			diff.normalize();
			edge = clickStart+diff*30;
		}
		else
			edge = Vector((float)posX,(float)posY,0);

		glBegin(GL_LINES);
			glVertex2f(clickStart.x,clickStart.y);
			glVertex2f(edge.x,edge.y);
		glEnd();

		glEnable(GL_TEXTURE_2D);
	}

	if(keys[VK_TAB] && !lkeys[VK_TAB])
		hud.toggleDocking();
	hud.update();
	float bulletTime = !hud.isDocking();

	shot.update(bulletTime);

	glLoadIdentity();
	
	glPushMatrix();
		glTranslatef(100,50,-5);
		tower1.render();
	glPopMatrix();

	glPushMatrix();
		glTranslatef(700,50,-5);
		tower2.render();
	glPopMatrix();

	game.Print(0,0,0,"%f - %f",fps,bulletTime);

	shot.render(bulletTime);
	hud.render(posX,posY);

	parts.RenderAll(bulletTime);

	return GameMod;
}

void GameModule::Shutdown()
{
}