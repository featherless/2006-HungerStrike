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

#ifndef GLWINDOW_H_
#define GLWINDOW_H_

#include <windows.h>
#include <gl/gl.h>

struct SETTINGS
{
	HINSTANCE hInstance;
	HWND hwnd;
	HDC			hDC;		// Private GDI Device Context
	HGLRC		hRC;		// Permanent Rendering Context
	BOOL fullscreen;
	char *tile;
	int width,height;
	int bits;
};

class glWindow
{
private:
	// Put variable definitions here.
public:
	SETTINGS Settings;

	BOOL CreateGLWindow(char* title, int width, int height, int bits, BOOL fullscreenflag);
	void KillGLWindow(GLvoid);
	// Place function definitions here.
};

#endif