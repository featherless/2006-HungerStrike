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

#include "TownHUD.h"

#include "GameDefs.h"

void TownHUD::render(int x,int y)
{
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();

	glColor3ub(127,207,121);

	glLoadIdentity();
	glTranslatef(offset,600-offset,20);
    
	glBegin(GL_QUADS);
		glTexCoord2f(1,0.0f); glVertex2f(   0, 256);
		glTexCoord2f(0,0.0f); glVertex2f(-256, 256);
		glTexCoord2f(0,0.1f); glVertex2f(-256,  0);
		glTexCoord2f(1,0.1f); glVertex2f(   0,  0);
	glEnd();

	glColor3f(1,1,1);

	glPushMatrix();
		glTranslatef(-128,128,1);
		glEnable(GL_TEXTURE_2D);
		SetTexture1(game.Textures[MINE]);
		glBegin(GL_QUADS);
			glTexCoord2f(1,0.0f); glVertex2f( 64, 32);
			glTexCoord2f(0,0.0f); glVertex2f(-64, 32);
			glTexCoord2f(0,1.0f); glVertex2f(-64,-32);
			glTexCoord2f(1,1.0f); glVertex2f( 64,-32);
		glEnd();

		glTranslatef(0,0,1);
		if(x>=offset-128-64 && x<=offset-128+64 &&
		   y>=600-offset+128-32 && y<=600-offset+128+32)
		{
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_LINE_LOOP);
				glVertex2f(-64,32);
				glVertex2f( 64,32);
				glVertex2f( 64,-32);
				glVertex2f(-64,-32);
			glEnd();
		}
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-64,224,1);
		glEnable(GL_TEXTURE_2D);
		SetTexture1(game.Textures[STORAGE]);
		glBegin(GL_QUADS);
			glTexCoord2f(1,0.0f); glVertex2f( 64, 32);
			glTexCoord2f(0,0.0f); glVertex2f(-64, 32);
			glTexCoord2f(0,1.0f); glVertex2f(-64,-32);
			glTexCoord2f(1,1.0f); glVertex2f( 64,-32);
		glEnd();

		glTranslatef(0,0,1);
		if(x>=offset-64-64 && x<=offset-64+64 &&
		   y>=600-offset+224-32 && y<=600-offset+224+32)
		{
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_LINE_LOOP);
				glVertex2f(-64,32);
				glVertex2f( 64,32);
				glVertex2f( 64,-32);
				glVertex2f(-64,-32);
			glEnd();
		}
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-192,224,1);
		glEnable(GL_TEXTURE_2D);
		SetTexture1(game.Textures[HOUSES]);
		glBegin(GL_QUADS);
			glTexCoord2f(1,0.0f); glVertex2f( 64, 32);
			glTexCoord2f(0,0.0f); glVertex2f(-64, 32);
			glTexCoord2f(0,1.0f); glVertex2f(-64,-32);
			glTexCoord2f(1,1.0f); glVertex2f( 64,-32);
		glEnd();

		glTranslatef(0,0,1);
		if(x>=offset-192-64 && x<=offset-192+64 &&
		   y>=600-offset+224-32 && y<=600-offset+224+32)
		{
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_LINE_LOOP);
				glVertex2f(-64,32);
				glVertex2f( 64,32);
				glVertex2f( 64,-32);
				glVertex2f(-64,-32);
			glEnd();
		}
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-224,128,1);
		glEnable(GL_TEXTURE_2D);
		SetTexture1(game.Textures[FIELD]);
		glBegin(GL_QUADS);
			glTexCoord2f(1,0.0f); glVertex2f( 32, 64);
			glTexCoord2f(0,0.0f); glVertex2f(-32, 64);
			glTexCoord2f(0,1.0f); glVertex2f(-32,-64);
			glTexCoord2f(1,1.0f); glVertex2f( 32,-64);
		glEnd();

		glTranslatef(0,0,1);
		if(x>=offset-224-32 && x<=offset-224+32 &&
		   y>=600-offset+128-64 && y<=600-offset+128+64)
		{
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_LINE_LOOP);
				glVertex2f(-32,64);
				glVertex2f( 32,64);
				glVertex2f( 32,-64);
				glVertex2f(-32,-64);
			glEnd();
		}
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-64,32,1);
		glEnable(GL_TEXTURE_2D);
		SetTexture1(game.Textures[STUDY]);
		glBegin(GL_QUADS);
			glTexCoord2f(1,0.0f); glVertex2f( 64, 32);
			glTexCoord2f(0,0.0f); glVertex2f(-64, 32);
			glTexCoord2f(0,1.0f); glVertex2f(-64,-32);
			glTexCoord2f(1,1.0f); glVertex2f( 64,-32);
		glEnd();

		glTranslatef(0,0,1);
		if(x>=offset-64-64 && x<=offset-64+64 &&
		   y>=600-offset+32-32 && y<=600-offset+32+32)
		{
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_LINE_LOOP);
				glVertex2f(-64,32);
				glVertex2f( 64,32);
				glVertex2f( 64,-32);
				glVertex2f(-64,-32);
			glEnd();
		}
	glPopMatrix();

	glPopMatrix();
}

void TownHUD::update()
{
	if(docking)
	{
		offset+=g_speed*10;
		if(offset>256)
			offset=256;
	}
	else
	{
		offset-=g_speed*10;
		if(offset<0)
			offset=0;
	}
}

void TownHUD::toggleDocking()
{
	docking = !docking;
}