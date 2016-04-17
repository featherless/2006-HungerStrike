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

#include "FPS.h"
#include <windows.h>

#pragma comment(lib,"winmm.lib")

// Programmed by Jeff Verkoeyen

UINT frames=0;
double fps=60.0;
double g_speed = 1.0;
LARGE_INTEGER freq;

LARGE_INTEGER oldTimerVal;

void InitFPS()
{
  QueryPerformanceFrequency(&freq);

  freq.QuadPart /= UPDATESPERSEC;

  LARGE_INTEGER newTimerVal;

  QueryPerformanceCounter(&newTimerVal); 

  oldTimerVal = newTimerVal;
}

void StartFPS()
{
	frames++;
}

void CalcFPS()
{
  LARGE_INTEGER newTimerVal;

  QueryPerformanceCounter(&newTimerVal); 

  if(oldTimerVal.QuadPart > newTimerVal.QuadPart)
	{
    oldTimerVal = newTimerVal;
    frames = 0;
	}

  LONGLONG diff = newTimerVal.QuadPart - oldTimerVal.QuadPart;
  if (diff > freq.QuadPart)
  {
    double perc = (double)diff / (double)freq.QuadPart;
    double framePerc = ((double)frames / perc) / ((double)BASEFRAMESPEED / (double)UPDATESPERSEC);
    fps = framePerc * (double)BASEFRAMESPEED;
    g_speed = (double)BASEFRAMESPEED / fps;
    frames = 0;

    oldTimerVal = newTimerVal;
  }
}