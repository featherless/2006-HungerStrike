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

#ifndef GAME_H_
#define GAME_H_

struct Wizard
{
	float x,y;
	float xv;
	int facing;
	int health;
	unsigned long injuredTick;
	unsigned int score;
	unsigned int kills;

	float hover;
};

struct Cloud
{
	float x,y,xv;
	int ID;
};

struct scoreText
{
	float x,y;
	unsigned long timer;
	unsigned int score;

	scoreText* next;
};

struct levelScores
{
	int score;
	int comboMax;
};

struct Boss
{
	int life;
	float x,y;
	float dir;
	int animation;
	int attackType;
	int pathCount;
	unsigned long animTick;
	unsigned long pathTick;
};

enum DifficultyLevels { EASY, NORMAL, HARD, CRAZY, NUMDIFFLEVELS };
const char DifficultyLevelsText[NUMDIFFLEVELS][16]={"Easy", "Normal", "Hard", "Crazy" };
extern int difficulty;
extern int gesturedifficulty;
extern levelScores level1Score;
extern levelScores level2Score;

#endif