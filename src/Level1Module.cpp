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

#include "Level1Module.h"
#include "GameDefs.h"

Module* Level1Mod;

#define GESTURESIZE		32

// Level 1 spells!
const Directions Spell1[] = { TOP, TOPLEFT, LEFT, BOTTOMLEFT, BOTTOM, BOTTOMRIGHT, RIGHT, TOPRIGHT, TOP, END };
const Directions Spell2[] = { TOPLEFT, BOTTOMRIGHT, BREAK, TOPRIGHT, BOTTOMLEFT, END };
const Directions Spell3[] = { TOP, BOTTOM, RIGHT, LEFT, TOP, END };
const Directions Spell4[] = { BOTTOM, BOTTOMLEFT, BREAK, TOP, TOPRIGHT, RIGHT, LEFT, END };
const Directions Spell5[] = { BOTTOM, BOTTOMLEFT, BREAK, TOP, TOPRIGHT, RIGHT, LEFT, END };

const Directions* Spells[]=
{
	Spell1, Spell2, Spell3, Spell4,
};

void Level1Module::GiveData(void* data)
{}

void Level1Module::Initialize()
{
	parts.ShutdownEngine();

	index = 0;
	numSpells = sizeof(Spells)/sizeof(Directions*);
	spell = (Directions*)Spells[rand()%numSpells];

	glEnable(GL_BLEND);

	clickStart = Vector(0,0,0);


	GestureTrailData data;
	data.radius=1;
	data.size=16;
	gestureTrail=new GestureTrail;
	gestureTrail->SetNumParts(500);
	gestureTrail->SetColor(0,255,0);
	gestureTrail->GiveData(&data);
	gestureTrail->SetStartVect(&clickStart);
	parts.giveChild(gestureTrail,&gestureTrailID);

	lastTick = GetTickCount();
}

Module* Level1Module::ExecuteModule()
{
	posX=(int)((mouse.X-game.newPosX));
	posY=(int)((600-(mouse.Y-game.newPosY)));

	Directions highlight;
	Directions next;
	float offset;
	highlight = spell[index];
	next	  = spell[index+1];

	if(mouse.L)
	{
		float closest = VectDist(Vector((float)posX,(float)posY,0),(Vector)dirVectors[TOP]+Vector(400,300,0));
		Directions closestDir = TOP;
		for(int a=1;a<BREAK;a++)
		{
			float dist = VectDist(Vector((float)posX,(float)posY,0),(Vector)dirVectors[a]+Vector(400,300,0));
			if(dist < closest)
			{
				closestDir = (Directions)a;
				closest = dist;
			}
		}
		if(!mouse.hL)
		{
			mouse.hL=true;
			clickedDirection=closestDir;
		}
		else
		{
			if(clickedDirection == highlight && closestDir == next)
			{
				index++;
				clickedDirection = next;
				if(spell[index+1] == END )
				{
					index = 0;
					clickedDirection = INVALID;
					spell = (Directions*)Spells[rand()%numSpells];
				}
			}
		}
	}
	else
	{
		if(next == BREAK)
			index+=2;
		else if(index>1 && spell[index-1]!=BREAK)
			index=0;
	}

	if(!mouse.L)
	{
		clickedDirection = INVALID;
	}

	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	SetTexture1(game.Textures[GESTURES]);

	glColor4f(1,1,1,1);
	glPushMatrix();
	glTranslatef(400,300,0);
	if(clickedDirection != TOP)
	{
		glPushMatrix();
			glTranslatef(0,200,0);

			offset = 0.0f;
			if(highlight==TOP)	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}

	if(clickedDirection != BOTTOM)
	{
		glPushMatrix();
			glTranslatef(0,-200,0);

			offset = 0.0f;
			if(highlight==BOTTOM)	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}

	if(clickedDirection != LEFT)
	{
		glPushMatrix();
			glTranslatef(-200,0,0);

			offset = 0.0f;
			if(highlight==LEFT)	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}

	if(clickedDirection != RIGHT)
	{
		glPushMatrix();
			glTranslatef(200,0,0);

			offset = 0.0f;
			if(highlight==RIGHT)	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}

	if(clickedDirection != TOPRIGHT)
	{
		glPushMatrix();
			glTranslatef(141,141,0);

			offset = 0.0f;
			if(highlight==TOPRIGHT)	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}

	if(clickedDirection != TOPLEFT)
	{
		glPushMatrix();
			glTranslatef(-141,141,0);

			offset = 0.0f;
			if(highlight==TOPLEFT)	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}

	if(clickedDirection != BOTTOMLEFT)
	{
		glPushMatrix();
			glTranslatef(-141,-141,0);

			offset = 0.0f;
			if(highlight==BOTTOMLEFT)	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}

	if(clickedDirection != BOTTOMRIGHT)
	{
		glPushMatrix();
			glTranslatef(141,-141,0);

			offset = 0.0f;
			if(highlight==BOTTOMRIGHT)	offset=0.5f;
			glBegin(GL_QUADS);
				glTexCoord2f(offset+0.5f,0.0f); glVertex2f( GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     0.0f); glVertex2f(-GESTURESIZE, GESTURESIZE);
				glTexCoord2f(offset,     1.0f); glVertex2f(-GESTURESIZE,-GESTURESIZE);
				glTexCoord2f(offset+0.5f,1.0f); glVertex2f( GESTURESIZE,-GESTURESIZE);
			glEnd();
		glPopMatrix();
	}
	glPopMatrix();

	if(next != INVALID && next != BREAK && GetTickCount()-lastTick>50)
	{
		Vector startLine = dirVectors[highlight];
		Vector endLine = dirVectors[next];

		Vector diff = endLine-startLine;
		float length = diff.length();

		if(length)
		{
			diff/=length;					// Normalize the vector

			Vector oldVal = startLine+Vector(400,300,0);
			for(float a=0;a<length;a+=length/25)
			{
				clickStart = oldVal + diff*a;
				gestureTrail->ForceActivateParts(5);
			}
		}
		lastTick=GetTickCount();
	}

	glLoadIdentity();
	SetTexture1(game.Textures[TOWER]);
	glPushMatrix();
		glTranslatef(64,0,0);

		glColor4f(1,1,1,1);
	    
		glBegin(GL_QUADS);
			glTexCoord2f(1,0.0f); glVertex2f( 32, 256);
			glTexCoord2f(0,0.0f); glVertex2f(-32, 256);
			glTexCoord2f(0,1.0f); glVertex2f(-32,  0);
			glTexCoord2f(1,1.0f); glVertex2f( 32,  0);
		glEnd();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(736,0,0);

		glColor4f(1,1,1,1);
	    
		glBegin(GL_QUADS);
			glTexCoord2f(1,0.0f); glVertex2f( 32, 256);
			glTexCoord2f(0,0.0f); glVertex2f(-32, 256);
			glTexCoord2f(0,1.0f); glVertex2f(-32,  0);
			glTexCoord2f(1,1.0f); glVertex2f( 32,  0);
		glEnd();
	glPopMatrix();

	int numRend=parts.RenderAll(1);

	glColor4f(1,1,1,1);
	game.Print(0,0,0,"FPS: %f %d",fps,numRend);

	return Level1Mod;
}

void Level1Module::Shutdown()
{
}
