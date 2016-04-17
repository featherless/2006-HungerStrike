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

#ifndef _CAMERA_H_
#define _CAMERA_H_


// Programmed by Jeff Verkoeyen


//////////////////////////////////////////////////////////////////
///////////////////////// INCLUDES ///////////////////////////////
//////////////////////////////////////////////////////////////////
#include <cVector.h>
#include <windows.h>
#include <gl\glu.h>



//////////////////////////////////////////////////////////////////
/////////////////////// ENUMERATIONS /////////////////////////////
//////////////////////////////////////////////////////////////////
enum CamTypes { FIRSTPERSON, FLYAROUND, STAYPUT };



//////////////////////////////////////////////////////////////////
///////////////////////// EXTERNALS //////////////////////////////
//////////////////////////////////////////////////////////////////
extern int g_width,g_height;		// window height/width



//////////////////////////////////////////////////////////////////
////////////////////////// CLASSES ///////////////////////////////
//////////////////////////////////////////////////////////////////
class Camera
{
private:
public:
	float updownRot;
	CamTypes CamType;
	Vector pos,dir,up,strafe;
	Vector oldPos;
	Vector vel;

	inline void Set(Vector Pos,Vector Dir,Vector Up=Vector(0.0,1.0f,0.0f))
	{
		updownRot=0;
		pos=Pos;
		dir=Dir;
		dir.normalize();
		up=Up;
		up.normalize();
		strafe=CrossProduct(dir,up);
		SetCursorPos(g_width>>1,g_height>>1);

		vel=Vector(0,0,0);
	}
	inline void SetCamera()		// Only call this once at the beginning of your code
	{ Vector view=pos+dir; gluLookAt(pos.x,pos.y,pos.z,view.x,view.y,view.z,up.x,up.y,up.z); }

	Vector MoveDir(float speed);
	Vector StrafeDir(float speed);
	void HandleMouse();
	void RotateAxis(float angle,Vector axis);
	void RenderVectors();
};



// By Jeff Verkoeyen
#endif		// #ifndef _CAMERA_H_