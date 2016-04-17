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

#include <TextureLoaderDevil.h>
#include <olectl.h>
#include <math.h>
#include <stdio.h>
#include <gl\glu.h>


// Programmed by Jeff Verkoeyen

int lastID=-10000;
int lastID2=-10000;

int LoadTexture(char* filename)
{
	int imageID=ilutGLLoadImage(filename);

	if(ilGetError()==IL_COULD_NOT_OPEN_FILE)
		return -1;
	return imageID;
}

void CreateRenderTexture(UINT *texture, int size, int channels, int type)
{
	unsigned int *pTexture = new unsigned int[size*size*channels];
	ZeroMemory(pTexture,sizeof(unsigned int)*size*size*channels);

	glGenTextures(1, texture);
	SetTexture1(*texture);

	glTexImage2D(GL_TEXTURE_2D, 0, channels, size, size, 0, type, GL_UNSIGNED_INT, pTexture);						

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	delete [] pTexture;
}