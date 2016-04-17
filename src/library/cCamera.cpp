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

//////////////////////////////////////////////////////////////////
///////////////////////// INCLUDES ///////////////////////////////
//////////////////////////////////////////////////////////////////
#include <cCamera.h>
#include <fps.h>
#include <gl\gl.h>


// Programmed by Jeff Verkoeyen


//////////////////////////////////////////////////////////////////
///////////////////////// CONSTANTS //////////////////////////////
//////////////////////////////////////////////////////////////////
const Vector cg_up(0,1,0);		// Constant global up vector



//////////////////////////////////////////////////////////////////
///////////////////////// FUNCTIONS //////////////////////////////
//////////////////////////////////////////////////////////////////
// Renders our vectors so we can see what the camera is doing
void Camera::RenderVectors()
{
	glPushMatrix();
		glTranslatef(pos.x,pos.y,pos.z);
		glBegin(GL_LINES);
			glColor3f(1,0,0);
			glVertex3f(0,0,0);
			glVertex3f(dir.x*50,dir.y*50,dir.z*50);

			glColor3f(0,1,0);
			glVertex3f(0,0,0);
			glVertex3f(up.x*50,up.y*50,up.z*50);

			glColor3f(0,0,1);
			glVertex3f(0,0,0);
			glVertex3f(strafe.x*50,strafe.y*50,strafe.z*50);
		glEnd();
	glPopMatrix();
}

// Rotates around an arbitrary axis, made this simplified equation, seems to work
void Camera::RotateAxis(float angle,Vector axis)
{
	float sinTheta=(float)sin(angle);

	dir+=CrossProduct(dir,axis)*sinTheta;
	dir.normalize();
}

// Handles mouse movement automatically
void Camera::HandleMouse()
{
	if(CamType==STAYPUT)
		return;
	int midx=g_width>>1;
	int midy=g_height>>1;
	POINT mouse;
	GetCursorPos(&mouse);

// No point in calculating if no difference occurred
	if(mouse.x==midx && mouse.y==midy)
		return;

	float diffx=(float)(mouse.x-midx);
	float diffy=(float)(mouse.y-midy);
	SetCursorPos(midx,midy);

	float changex=(diffx/300)*g_speed;
	float changey=(diffy/300)*g_speed;

	float oldRot=updownRot;
	updownRot+=changey;
	if(updownRot<-1)
	{
		updownRot=-1;
		changey-=(updownRot+1);
	}
	else if(updownRot>1)
	{
		updownRot=1;
		changey+=(updownRot-1);
	}
	if(oldRot>-1 && oldRot<1)
		RotateAxis(changey,strafe);
// We already have the x plane vector to rotate up and down around,
//  as our strafe vector, so let's calculate that first
	RotateAxis(changex,Vector(0,1,0));
	strafe=CrossProduct(dir,cg_up);
	strafe.normalize();

// This is optional, mainly for debugging purposes, we *could* use it for
// rotating around, but that would get messed up, thus the reason why we use
// the cg_up variable which is a camera global for "up"
	up=CrossProduct(strafe,dir);
}

// Move either forward or backward
Vector Camera::MoveDir(float speed)
{
	oldPos=pos;
	if(CamType==STAYPUT)
		return Vector(0,0,0);
	speed*=g_speed;
	Vector view=dir;
	if(CamType==FIRSTPERSON)
		view.y=0;
	view.normalize();
	pos+=view*speed;
	return view;
}

// Strafe side to side
Vector Camera::StrafeDir(float speed)
{
	oldPos=pos;
	if(CamType==STAYPUT)
		return Vector(0,0,0);
	speed*=g_speed;
	pos+=strafe*speed;
	return strafe;
}

// By Jeff Verkoeyen