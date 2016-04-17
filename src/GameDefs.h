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

#ifndef GAMEDEFS_H_
#define GAMEDEFS_H_

#include <glWindow.h>
#include <TextureLoaderDevil.h>
#include "fps.h"

#define SCREENX		800
#define SCREENY		600

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLu32.lib")
#pragma comment(lib, "fmodex_vc.lib")


#define KEY_DOWN(vk_code)	((GetAsyncKeyState((vk_code))&0x8000 ? 1:0))

#ifndef CLEAN
#define CLEAN(value) { if(value) { delete [] value; value=NULL; } }
#endif

#ifndef RESIZE
#define RESIZE(value,type,newSize,Original) { if(!Original) value=new type[(newSize)]; else value=(type*)realloc((void*)(value),sizeof(type)*(newSize)); }
#endif



#define FONT_HEIGHT	48




/////////////////////////// ENUMERATIONS ///////////////////////////
enum WindowRes {_800x600, _1024x768, _1280x1024, _1600x1200, _1920x1440, _2040x1536, NUMRES };

typedef struct WINDETSDEF
{
	int Width,Height;
	int bpp;
	BOOL FullScreen;
} WINDETS, *LPWINDETS;

const wchar_t cWindowRes[NUMRES][16]=
{
	L"800x600",
	L"1024x768",
	L"1280x1024",
	L"1600x1200",
	L"1920x1440",
	L"2040x1536"
};
const int iWindowRes[NUMRES][2]=
{
	{800,600},
	{1024,768},
	{1280,1024},
	{1600,1200},
	{1920,1440},
	{2040,1536}
};

/* Global game data structure */
struct GameData
{
	unsigned int level;
	int highScores1[4];
	int highCombos1[4];
};

extern GameData gameDat;

BOOL CALLBACK InitDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


enum textureIDs { FIREBALL, GESTURES, PARTICLE,
				  WIZARD, GRASS, PILLAR,
				  ZOMBIE, RAIN, FIREBALLICON, MOUNTAIN,
				  GIBBS, ROCK, HEALTH, BOSS1, TITLE, INSTRUCTIONS1,
				  BOSS2, TOMATO, SHIELD, LOSE, WIN,
				  NUMTEXTURES };

const char textureNames[NUMTEXTURES][48]=
{
	"images\\fireball.png",
	"images\\gestures.png",
	"images\\particle.png",
	"images\\wizard.png",
	"images\\grass.png",
	"images\\pillar.png",
	"images\\zombie.png",
	"images\\rain.png",
	"images\\fireballIcon.png",
	"images\\mountain.png",
	"images\\gibbs.png",
	"images\\rock.png",
	"images\\health.png",
	"images\\boss1.png",
	"images\\title.png",
	"images\\instructions1.png",
	"images\\boss2.png",
	"images\\tomato.png",
	"images\\shield.png",
	"images\\lose.png",
	"images\\win.png",
};

#define MAX_CHARS	256
class GameClass
{
private:
	WINDETS Dets;
	HFONT hOldFont;
	UINT StdFontBase;
	UINT Std3DFontBase;

	GLYPHMETRICSFLOAT GlyphInfo[MAX_CHARS];
public:
	UINT base;
	unsigned int Textures[NUMTEXTURES];
	UINT FontText;
	UINT newPosX,newPosY;
	UINT newSizeX,newSizeY;

	BOOL InitGameControls(void);
	BOOL InitGame(void);

	int LoadStandardFont(char* name,int height,int flags=FW_BOLD,int italic=0);
	int LoadStandardFontIntoList(char* name,int height,unsigned int* list,int flags=FW_BOLD,int italic=0);
	int LoadStandard3DFont(char* name,float extrude,int flags=FW_BOLD,int italic=0);
	void Print(float x,float y,char* Text,...);
	void Print3D(float x,float y,float z,float scale,char* Text,...);
	void PrintList(float x,float y,unsigned int list,char* Text,...);

	void BuildFont();

	void Print(int x,int y,UINT Set,char* Text,...);
	void Print(int x,int y,float sx,float sy,UINT Set,char* Text,...);
	void PrintAndSwap(char* Text,...);

	glWindow GLWindow;
	void ResizeWindow(UINT width,UINT height);

	inline void GameSwapBuffers() {SwapBuffers(GLWindow.Settings.hDC);};

	BOOL ShutdownGame(void);
};

extern GameClass game;

#endif