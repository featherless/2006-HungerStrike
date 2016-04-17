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

// INCLUDES //
//#include "GameModule.h"
//#include "GesturesModule.h"
//#include "Level1Module.h"
#include "GameLevel1Module.h"
#include "GameLevel2Module.h"
#include "GameLevel3Module.h"
//#include "NewGameModule.h"
#include "LoseModule.h"
#include "GameWinModule.h"
#include "MainMenuModule.h"
#include "WinLevel1Module.h"
#include "WinLevel2Module.h"
#include "InstructionsModule.h"
#include "CreditsModule.h"


#include "SFXEngine.h"

#include "fps.h"
#include "GameDefs.h"

// GLOBALS //
Mouse mouse;
bool done=false;
char GlobalDir[MAX_PATH];

bool active=true;

Module* gamemod=NULL;
FMOD::System *fmodSystem;

int NoInit=0;
int NoReset=0;

bool DrawScene()
{
	CalcFPS();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Module* oldMod=gamemod;
	if((gamemod=gamemod->ExecuteModule())!=oldMod)
	{
		if(!gamemod)
		{
			gamemod=oldMod;
			done=true;
		}
		else
		{
			lastMod=oldMod;
			if(!NoReset)	lastMod->Shutdown();
			NoReset=FALSE;
			if(!NoInit)		gamemod->Initialize();
			NoInit=FALSE;
		}
	}

	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_ACTIVATE:
			if(!HIWORD(wParam))		active=true;
			else					active=false;
			switch(LOWORD(wParam))
			{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				active=true;
				break;
			case WA_INACTIVE:
				active=false;
				break;
			}
			return 0;
		case WM_SYSCOMMAND:
			switch(wParam)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN:
			keys[wParam]=TRUE;
			return 0;
		case WM_KEYUP:
			keys[wParam]=FALSE;
			return 0;
		case WM_MOUSEMOVE:
			mouse.X=LOWORD(lParam);
			mouse.Y=HIWORD(lParam);
			return 0;
		case WM_LBUTTONDOWN:
			mouse.L=true;
			return 0;
		case WM_LBUTTONUP:
			mouse.L=false;
			mouse.hL=false;
			return 0;
		case WM_RBUTTONDOWN:
			mouse.R=true;
			return 0;
		case WM_RBUTTONUP:
			mouse.R=false;
			mouse.hR=false;
			return 0;
		case WM_MBUTTONDOWN:
			mouse.M=true;
			return 0;
		case WM_MBUTTONUP:
			mouse.M=false;
			mouse.hM=false;
			return 0;
		case WM_SIZE:
			game.ResizeWindow(LOWORD(lParam),HIWORD(lParam));
			return 0;
	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void Init()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.2f,0.2f,0.2f, 0.5f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0);

	glEnable(GL_TEXTURE_2D);

	srand(GetTickCount());

	ilInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	msg.wParam=0;

	if(!game.InitGameControls())
		return 0;
	Init();
	if(!game.InitGame())
		return 0;

	//GameMod=new GameModule();
	//GesturesMod=new GesturesModule();
	//Level1Mod=new Level1Module();
	GameLevel1Mod=new GameLevel1Module();
	GameLevel2Mod=new GameLevel2Module();
	GameLevel3Mod=new GameLevel3Module();
	//NewGameMod=new NewGameModule();
	LoseMod=new LoseModule();
	WinLevel1Mod=new WinLevel1Module();
	WinLevel2Mod=new WinLevel2Module();
	MainMenuMod=new MainMenuModule();
	InstructionsMod=new InstructionsModule();
	GameWinMod=new GameWinModule();
	CreditsMod=new CreditsModule();

	timeBeginPeriod(1);

//	cFMODInitSystem();

	gamemod=MainMenuMod;

	gameDat.level=0;


	FMOD_RESULT result;
	result = FMOD::System_Create(&fmodSystem);		// Create the main system object.
	if (result != FMOD_OK)
	{
		MessageBox(game.GLWindow.Settings.hwnd,L"FMOD error creating sound system!",L"Error",MB_OK);
		return 0;
	}

	result = fmodSystem->init(64, FMOD_INIT_NORMAL, 0);	// Initialize FMOD.
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}

	sfx.LoadSFXs();

	FILE* file;
	errno_t error = fopen_s(&file,"data.dat","rb");
	if(!error)
	{
		unsigned char chunk[256];
		fread(chunk,256,1,file);
		fclose(file);
		unsigned char offset=chunk[0];
		gameDat.level=65535-*((int*)(chunk+offset));

		gameDat.highCombos1[0]=65535-*((int*)(chunk+offset+4));
		gameDat.highCombos1[1]=65535-*((int*)(chunk+offset+8));
		gameDat.highCombos1[2]=65535-*((int*)(chunk+offset+12));
		gameDat.highCombos1[3]=65535-*((int*)(chunk+offset+16));

		gameDat.highScores1[0]=65535-*((int*)(chunk+offset+20));
		gameDat.highScores1[1]=65535-*((int*)(chunk+offset+24));
		gameDat.highScores1[2]=65535-*((int*)(chunk+offset+28));
		gameDat.highScores1[3]=65535-*((int*)(chunk+offset+32));

	}
	else
	{
		gameDat.level=0;
		gameDat.highCombos1[0]=0;
		gameDat.highCombos1[1]=0;
		gameDat.highCombos1[2]=0;
		gameDat.highCombos1[3]=0;

		gameDat.highScores1[0]=0;
		gameDat.highScores1[1]=0;
		gameDat.highScores1[2]=0;
		gameDat.highScores1[3]=0;
	}

	gamemod->Initialize();

	while(!done)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message==WM_QUIT)
				done=TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if(!DrawScene())
				done=TRUE;
			else
			{
				game.GameSwapBuffers();
				memcpy(lkeys,keys,sizeof(bool)*256);
			}
		}
	}
	gamemod->Shutdown();
	game.ShutdownGame();

	timeEndPeriod(1);

	//delete GameMod;
	//delete GesturesMod;
	delete GameLevel1Mod;
	delete GameLevel2Mod;
	delete GameLevel3Mod;
	//delete Level1Mod;
	//delete NewGameMod;
	delete LoseMod;
	delete GameWinMod;
	delete WinLevel1Mod;
	delete WinLevel2Mod;
	delete MainMenuMod;
	delete InstructionsMod;
	delete CreditsMod;

	error = fopen_s(&file,"data.dat","rb");
	if(!error)
	{
		unsigned char chunk[256];
		unsigned char offset=rand()%128+1;
		chunk[0]=offset;
		for(int a=1;a<256;a++)
			chunk[a]=rand()%256;
		*((int*)(chunk+offset))=65535-gameDat.level;

		*((int*)(chunk+offset+4))=65535-gameDat.highCombos1[0];
		*((int*)(chunk+offset+8))=65535-gameDat.highCombos1[1];
		*((int*)(chunk+offset+12))=65535-gameDat.highCombos1[2];
		*((int*)(chunk+offset+16))=65535-gameDat.highCombos1[3];

		*((int*)(chunk+offset+20))=65535-gameDat.highScores1[0];
		*((int*)(chunk+offset+24))=65535-gameDat.highScores1[1];
		*((int*)(chunk+offset+28))=65535-gameDat.highScores1[2];
		*((int*)(chunk+offset+32))=65535-gameDat.highScores1[3];

		fwrite(chunk,256,1,file);
		fclose(file);
	}
//	cFMODCloseSystem();

	return((int)msg.wParam);
}