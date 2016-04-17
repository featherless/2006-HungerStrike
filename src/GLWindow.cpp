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

#include "glWindow.h"

#define SHOWCURSOR		true

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void glWindow::KillGLWindow(GLvoid)
{
	if (Settings.fullscreen)
	{
		ChangeDisplaySettings(NULL,0);
		ShowCursor(TRUE);
	}

	if (Settings.hRC)
	{
		if (!wglMakeCurrent(NULL,NULL))
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);

		if (!wglDeleteContext(Settings.hRC))
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		Settings.hRC=NULL;
	}

	if (Settings.hDC && !ReleaseDC(Settings.hwnd,Settings.hDC))
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		Settings.hDC=NULL;
	}

	if (Settings.hwnd && !DestroyWindow(Settings.hwnd))
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		Settings.hwnd=NULL;
	}

	if (!UnregisterClass("OpenGL",Settings.hInstance))
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		Settings.hInstance=NULL;
	}
}

BOOL glWindow::CreateGLWindow(char* title, int width, int height, int bits, BOOL fullscreenflag)
{
	GLuint		PixelFormat;
	WNDCLASS	wc;
	DWORD		dwExStyle;
	DWORD		dwStyle;
	RECT		WindowRect;
	WindowRect.left=(long)0;
	WindowRect.right=(long)width;
	WindowRect.top=(long)0;
	WindowRect.bottom=(long)height;

	Settings.fullscreen=fullscreenflag;

	Settings.hInstance	= GetModuleHandle(NULL);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= Settings.hInstance;
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= "OpenGL";

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	
	if (Settings.fullscreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= width;
		dmScreenSettings.dmPelsHeight	= height;
		dmScreenSettings.dmBitsPerPel	= bits;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				Settings.fullscreen=FALSE;
			}
			else
			{
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	if (Settings.fullscreen)
	{
		dwExStyle=WS_EX_APPWINDOW;
		dwStyle=WS_POPUP;
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle=WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	if (!(Settings.hwnd=CreateWindowEx(	dwExStyle,
								"OpenGL",
								title,
								dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								0, 0,
								WindowRect.right-WindowRect.left,
								WindowRect.bottom-WindowRect.top,
								NULL,
								NULL,
								Settings.hInstance,
								NULL)))
	{
		KillGLWindow();
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	static	PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		bits,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	
	if (!(Settings.hDC=GetDC(Settings.hwnd)))
	{
		KillGLWindow();
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(PixelFormat=ChoosePixelFormat(Settings.hDC,&pfd)))
	{
		KillGLWindow();
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	if(!SetPixelFormat(Settings.hDC,PixelFormat,&pfd))
	{
		KillGLWindow();
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(Settings.hRC=wglCreateContext(Settings.hDC)))
	{
		KillGLWindow();
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	if(!wglMakeCurrent(Settings.hDC,Settings.hRC))
	{
		KillGLWindow();
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	ShowWindow(Settings.hwnd,SW_SHOW);
	SetForegroundWindow(Settings.hwnd);
	SetFocus(Settings.hwnd);

	ShowCursor(SHOWCURSOR);

	return TRUE;
}