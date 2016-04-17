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

#include "Tower.h"
#include "GameDefs.h"

void Tower::render()
{
	SetTexture1(game.Textures[TOWER]);

	glColor4f(1,1,1,1);
    
	glBegin(GL_QUADS);
		glTexCoord2f(1,0.5f);  glVertex2f( 64, 128);
		glTexCoord2f(0,0.5f);  glVertex2f(-64, 128);
		glTexCoord2f(0,0.75f); glVertex2f(-64,  0);
		glTexCoord2f(1,0.75f); glVertex2f( 64,  0);

	for(float offset = 128; offset < 128+128*numSections; offset+=128)
	{
		glTexCoord2f(1,0.25f); glVertex2f( 64, offset+128);
		glTexCoord2f(0,0.25f); glVertex2f(-64, offset+128);
		glTexCoord2f(0,0.5f);  glVertex2f(-64, offset);
		glTexCoord2f(1,0.5f);  glVertex2f( 64, offset);
	}

		glTexCoord2f(1,0.0f);  glVertex2f( 64, offset+128);
		glTexCoord2f(0,0.0f);  glVertex2f(-64, offset+128);
		glTexCoord2f(0,0.25f); glVertex2f(-64, offset);
		glTexCoord2f(1,0.25f); glVertex2f( 64, offset);

	glEnd();
}