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

#ifndef TEXTURELOADER_H_
#define TEXTURELOADER_H_

// Programmed by Jeff Verkoeyen

#include <windows.h>
#include <gl\gl.h>
#ifdef _UNICODE
#undef _UNICODE
#define ENABLEUNICODE
#endif
#include <IL/il.h>
#include <IL/ilut.h>

#ifdef ENABLEUNICODE
#define _UNICODE
#endif		// ENABLEUNICODE

#pragma comment( lib, "DevIL.lib" )
#pragma comment( lib, "ILUT.lib" )

extern int lastID;
extern int lastID2;

int LoadTexture(char* filename);
void CreateRenderTexture(UINT *texture,int size,int channels,int type);

inline void SetTexture1(int ID)
{
  if (ID < 0)
    lastID = ID;
  else if(lastID!=ID)
	{
		glBindTexture(GL_TEXTURE_2D,ID);
		lastID=ID;
	}
}
inline void SetTexture2(int ID)
{
  if (ID < 0)
    lastID2 = ID;
  else if(lastID2!=ID)
	{
		glBindTexture(GL_TEXTURE_2D,ID);
		lastID2=ID;
	}
}

#endif