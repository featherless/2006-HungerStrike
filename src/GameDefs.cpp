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

#include "GameDefs.h"
#include "resource.h"
#include <stdio.h>
#include <gl/glu.h>

#define WINDOWTITLE		L"Hunger Strike!"

#define FONT_NAME	"Book Antiqua"

#define FONTWIDTH		16
#define SPACEWIDTH		16

GameData gameDat;
GameClass game;

BOOL GameClass::InitGameControls(void)
{
	LPWINDETS WinDet=(LPWINDETS)DialogBox(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_INIT),NULL,(DLGPROC)InitDialogProc);
	if(!WinDet)	return FALSE;

	Dets.bpp=WinDet->bpp;
	Dets.FullScreen=WinDet->FullScreen;
	Dets.Height=WinDet->Height;
	Dets.Width=WinDet->Width;

	if(!GLWindow.CreateGLWindow(WINDOWTITLE,Dets.Width,Dets.Height,Dets.bpp,Dets.FullScreen))
		return 0;

	ResizeWindow(Dets.Width,Dets.Height);
	return 1;
}

BOOL GameClass::InitGame(void)
{
	glEnable(GL_TEXTURE_2D);

	int tempID=LoadTexture("images\\font.png");
	if(tempID==-1)
		MessageBox(0,L"Error loading font.",L"Error",MB_OK);

	FontText=tempID;
	BuildFont();


//	if(!LoadStandardFont(FONT_NAME,FONT_HEIGHT))
//		MessageBox(0,"Error loading font.","Error",MB_OK);

//	if(!LoadStandard3DFont("Gyparody",0.4f))
//		MessageBox(0,"Error loading font.","Error",MB_OK);

	for(int a=0;a<NUMTEXTURES;a++)
	{
		PrintAndSwap("Loading %s", textureNames[a]);

		int tempID=LoadTexture((char *)textureNames[a]);
		Textures[a]=-1;
		if(tempID==-1)	MessageBoxW(0,(LPCWSTR)textureNames[a],L"Error loading image",MB_OK);
		else			Textures[a]=tempID;
		SetTexture1(Textures[a]);
	}

	return 1;
}

BOOL GameClass::ShutdownGame(void)
{
//	FSOUND_Close();
	glDeleteLists(StdFontBase,MAX_CHARS);
	SelectObject(GLWindow.Settings.hDC,hOldFont);

	GLWindow.KillGLWindow();
	return 1;
}

int GameClass::LoadStandardFont(char* name,int height,int flags,int italic)
{
	HFONT	hFont;

	StdFontBase=glGenLists(MAX_CHARS);

	size_t length=strlen(name);
	wchar_t* newStr = new wchar_t[length+1];
	mbtowc(newStr,name,length+1);
	hFont=CreateFont(	height,16,0,0,
						flags,
						italic,
						FALSE,FALSE,
						ANSI_CHARSET,
						OUT_TT_PRECIS,
						CLIP_DEFAULT_PRECIS,
						ANTIALIASED_QUALITY,
						FF_DONTCARE|DEFAULT_PITCH,
						newStr);
	delete [] newStr;

	if(!hFont)
		return 0;

	hOldFont=(HFONT)SelectObject(GLWindow.Settings.hDC,hFont);

	wglUseFontBitmaps(GLWindow.Settings.hDC,0,MAX_CHARS-1,StdFontBase);

	return 1;
}
int GameClass::LoadStandardFontIntoList(char* name,int height,unsigned int* list,int flags,int italic)
{
	HFONT	hFont;

	*list=glGenLists(MAX_CHARS);

	size_t length=strlen(name);
	wchar_t* newStr = new wchar_t[length+1];
	mbtowc(newStr,name,length+1);
	hFont=CreateFont(	height,0,0,0,
						flags,
						italic,
						FALSE,FALSE,
						ANSI_CHARSET,
						OUT_TT_PRECIS,
						CLIP_DEFAULT_PRECIS,
						ANTIALIASED_QUALITY,
						FF_DONTCARE|DEFAULT_PITCH,
						newStr);
	delete [] newStr;

	if(!hFont)
		return 0;

	hOldFont=(HFONT)SelectObject(GLWindow.Settings.hDC,hFont);

	wglUseFontBitmaps(GLWindow.Settings.hDC,0,MAX_CHARS-1,*list);

	return 1;
}

int GameClass::LoadStandard3DFont(char* name,float extrude,int flags,int italic)
{
	HFONT	hFont;

	Std3DFontBase=glGenLists(MAX_CHARS);

	size_t length=strlen(name);
	wchar_t* newStr = new wchar_t[length+1];
	mbtowc(newStr,name,length+1);
	hFont=CreateFont(	0,0,0,0,
						flags,
						italic,
						FALSE,FALSE,
						ANSI_CHARSET,
						OUT_TT_PRECIS,
						CLIP_DEFAULT_PRECIS,
						ANTIALIASED_QUALITY,
						FF_DONTCARE|DEFAULT_PITCH,
						newStr);
	delete [] newStr;
	if(!hFont)
		return 0;
	hOldFont=(HFONT)SelectObject(GLWindow.Settings.hDC,hFont);

	wglUseFontOutlines(	GLWindow.Settings.hDC,
						0,MAX_CHARS-1,
						Std3DFontBase,
						0,
						extrude,
						WGL_FONT_POLYGONS,
						GlyphInfo);
	return 1;

}

void GameClass::ResizeWindow(UINT width,UINT height)
{
	newPosX=newPosY=0;
	newSizeX=0,newSizeY=0;

	if(width<=SCREENX)		newSizeX=width;
	else
	{
		newSizeX=SCREENX;
		newPosX=width-SCREENX-(width-SCREENX)/2;
	}
	if(height<=SCREENY)		newSizeY=height;
	else
	{
		newSizeY=SCREENY;
		newPosY=height-SCREENY-(height-SCREENY)/2;
	}

	glViewport(newPosX,newPosY,newSizeX,newSizeY);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0f,SCREENX,0,SCREENY,-100.0f,100.0f);
//	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,1.0f,450.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void GameClass::PrintAndSwap(char* Text,...)
{
	if(!Text) return;
	char text[1024];
	va_list ap;
	va_start(ap,Text); _vsnprintf_s(text,1023,1023,Text,ap); va_end(ap);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Print(0,16,0,text);
	SwapBuffers(GLWindow.Settings.hDC);
}

void GameClass::Print(int x,int y,float sx,float sy,UINT Set,char* Text,...)
{
	if(!Text) return;

	glPushMatrix();
	SetTexture1(FontText);

	char text[1024];
	va_list ap;

	va_start(ap,Text); _vsnprintf_s(text,1023,1023,Text,ap); va_end(ap);

	if(Set>1) Set=1;

	glLoadIdentity();
	if(x==-1)	glTranslated(400-((strlen(text)/2)*SPACEWIDTH*sx),y,30);
	else		glTranslated(x,y,30);
	glScalef(sx,sy,1);
	glListBase(base-48+(128*Set));

	glCallLists((GLsizei)strlen(text),GL_UNSIGNED_BYTE, text);
	glPopMatrix();
}

void GameClass::PrintList(float x,float y,unsigned int list,char* Text,...)
{
	if(!Text) return;
	char text[1024];
	va_list ap;

	glPushMatrix();

	va_start(ap,Text); _vsnprintf_s(text,1023,1023,Text,ap); va_end(ap);

	glRasterPos3f(x,y,0);

	glPushAttrib(GL_LIST_BIT);
	glListBase(list);

	glCallLists(strlen(text),GL_UNSIGNED_BYTE,text);

	glPopAttrib();

	glPopMatrix();
}

void GameClass::Print(float x,float y,char* Text,...)
{
	if(!Text) return;
	char text[1024];
	va_list ap;

	glPushMatrix();

	va_start(ap,Text); _vsnprintf_s(text,1023,1023,Text,ap); va_end(ap);

	glRasterPos3f(x,y,0);

	glPushAttrib(GL_LIST_BIT);
	glListBase(StdFontBase);

	glCallLists(strlen(text),GL_UNSIGNED_BYTE,text);

	glPopAttrib();

	glPopMatrix();
}

void GameClass::Print3D(float x,float y,float z,float scale,char* Text,...)
{
	if(!Text) return;
	char text[1024];
	va_list ap;

	glPushMatrix();

	va_start(ap,Text); _vsnprintf_s(text,1023,1023,Text,ap); va_end(ap);

	glTranslatef(x,y,z);
	glScalef(scale,scale,scale);

	glPushAttrib(GL_LIST_BIT);
	glListBase(Std3DFontBase);

	glCallLists(strlen(text),GL_UNSIGNED_BYTE,text);

	glPopAttrib();

	glPopMatrix();
}

/*
int GLS[8]={GL_ZERO,GL_ONE,GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR,
GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_DST_ALPHA,GL_ONE_MINUS_DST_ALPHA};
const char GLN[8][32]=
{
	"GL_ZERO",
	"GL_ONE",
	"GL_SRC_COLOR",
	"GL_ONE_MINUS_SRC_COLOR",
	"GL_SRC_ALPHA",
	"GL_ONE_MINUS_SRC_ALPHA",
	"GL_DST_ALPHA",
	"GL_ONE_MINUS_DST_ALPHA",
};

extern bool keys[256],lkeys[256];*/
void GameClass::Print(int x,int y,UINT Set,char* Text,...)
{/*
	static int a=0;
	static int b=0;
	if(keys['A'] && !lkeys['A']) a++;
	if(keys['Z'] && !lkeys['Z']) a--;
	if(keys['S'] && !lkeys['S']) b++;
	if(keys['X'] && !lkeys['X']) b--;
	if(a<0)		a=7;
	if(a>7)		a=0;
	if(b<0)		b=7;
	if(b>7)		b=0;*/
	if(!Text) return;

	glPushMatrix();
	SetTexture1(FontText);

	char text[1024];
	va_list ap;

	va_start(ap,Text); _vsnprintf_s(text,1023,1023,Text,ap); va_end(ap);

	if(Set>1) Set=1;

//	sprintf(text,"%s %s",GLN[a],GLN[b]);


// GL_ZERO,GL_ONE_MINUS_SRC_COLOR   -- Inverted
// GL_DST_ALPHA,GL_ONE_MINUS_SRC_ALPHA

//	glBlendFunc(GLS[a],GLS[b]);
	glLoadIdentity();
	if(x==-1)	glTranslated(400-((strlen(text)/2)*SPACEWIDTH),y,30);
	else		glTranslated(x,y,30);
	glListBase(base-16+(128*Set));

	glCallLists((GLsizei)strlen(text),GL_UNSIGNED_BYTE, text);
	glPopMatrix();
}


void GameClass::BuildFont()
{
	base=glGenLists(256);

	float cx,cy;
	for(int a=0;a<256;a++)
	{
		cx=(float)(a%16)/16.0f;
		cy=(float)(a/16)/16.0f;

		glNewList(base+a,GL_COMPILE);
			glBegin(GL_QUADS);
				glTexCoord2f(cx,cy);
				glVertex2i(0,0);
				glTexCoord2f(cx+0.0625f,cy);
				glVertex2i(FONTWIDTH,0);
				glTexCoord2f(cx+0.0625f,cy-0.0625f);
				glVertex2i(FONTWIDTH,FONTWIDTH);
				glTexCoord2f(cx,cy-0.0625f);
				glVertex2i(0,FONTWIDTH);
			glEnd();
			glTranslated(SPACEWIDTH,0,0);
		glEndList();
	}
}







BOOL CALLBACK InitDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
	case WM_INITDIALOG:
		{
			for(UINT a=0;a<NUMRES;a++)	SendMessage(GetDlgItem(hwnd,IDC_RESOLUTION),CB_ADDSTRING,NULL,(LPARAM)cWindowRes[a]);

			SendMessage(GetDlgItem(hwnd,IDC_RESOLUTION),CB_SETCURSEL,0,NULL);

			SendMessage(GetDlgItem(hwnd,IDC_BITS),CB_ADDSTRING,NULL,(LPARAM)L"16");
			SendMessage(GetDlgItem(hwnd,IDC_BITS),CB_ADDSTRING,NULL,(LPARAM)L"24");
			SendMessage(GetDlgItem(hwnd,IDC_BITS),CB_ADDSTRING,NULL,(LPARAM)L"32");
			SendMessage(GetDlgItem(hwnd,IDC_BITS),CB_SETCURSEL,0,NULL);

			SendMessage(GetDlgItem(hwnd,IDC_FULLSCREEN),BM_SETCHECK,BST_CHECKED,NULL);
		}
		break;
	case WM_MOVE:
		InvalidateRect(hwnd,NULL,true);
		UpdateWindow(hwnd);
		return true;
		break;
    case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hwnd,NULL);
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_OK:
			{
				static WINDETS WinDet;		// Make this static so we don't lose focus

				int Selection=SendMessage(GetDlgItem(hwnd,IDC_RESOLUTION),CB_GETCURSEL,NULL,NULL);
				WinDet.Width=iWindowRes[Selection][0];
				WinDet.Height=iWindowRes[Selection][1];

				Selection=SendMessage(GetDlgItem(hwnd,IDC_BITS),CB_GETCURSEL,NULL,NULL);
				switch(Selection)
				{
				case 0:				// 16
					WinDet.bpp=16;
					break;
				case 1:				// 24
					WinDet.bpp=24;
					break;
				case 2:				// 32
					WinDet.bpp=32;
					break;
				}

				if(SendMessage(GetDlgItem(hwnd,IDC_FULLSCREEN),BM_GETCHECK,BST_CHECKED,NULL)==BST_CHECKED)
					WinDet.FullScreen=TRUE;
				else
					WinDet.FullScreen=FALSE;
				EndDialog(hwnd,(int)&WinDet);
			}
			return true;
			break;
		}
		break;
	}
	return FALSE;
}