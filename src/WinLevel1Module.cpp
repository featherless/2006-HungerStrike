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

#include "WinLevel1Module.h"
#include "GameDefs.h"
#include "Game.h"
#include "GameLevel2Module.h"

Module* WinLevel1Mod;

void WinLevel1Module::GiveData(void* data)
{}

void WinLevel1Module::Initialize()
{
	highScore=highCombo=0;
	if(level1Score.comboMax>gameDat.highCombos1[difficulty])
	{
		gameDat.highCombos1[difficulty]=level1Score.comboMax;
		highScore=1;
	}
	if(level1Score.score>gameDat.highScores1[difficulty])
	{
		gameDat.highScores1[difficulty]=level1Score.score;
		highScore=1;
	}
}

Module* WinLevel1Module::ExecuteModule()
{
	int offset=400;
	game.Print(-1,offset-=16,0,"And with his final gesture, Ted the Wizard");
	game.Print(-1,offset-=16,0,"vanquished the small town of zombies.");
	game.Print(-1,offset-=48,0,"Score: %d",level1Score.score);
	game.Print(-1,offset-=16,0,"Highest Combo: %d",level1Score.comboMax);
	if(highScore)
		game.Print(-1,offset-=32,0,"New high score/");
	if(highCombo)
		game.Print(-1,offset-=32,0,"New high combo/");
	game.Print(-1,offset-=48,0,"Press space to continue to level 2");

	if(keys[VK_SPACE] && !lkeys[VK_SPACE])
		return GameLevel2Mod;
	return WinLevel1Mod;
}

void WinLevel1Module::Shutdown()
{
}
