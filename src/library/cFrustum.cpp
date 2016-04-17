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

#include "cFrustum.h"
#include <windows.h>
#include <gl\gl.h>

// Programmed by Jeff Verkoeyen

Frustum frustum;

#define FIRST_OBJECT_ID  3								

enum FrustumSide { RIGHT,LEFT,BOTTOM,TOP,BACK,FRONT }; 

int Frustum::boxTest(Vector* corner1,Vector* corner2)
{
	for(int a=0;a<6;a++)
	{
		if(DotProduct(&frustum[a].normal,corner1)+frustum[a].distance>0)  continue;
		if(frustum[a].normal.x*corner2->x + frustum[a].normal.y*corner1->y + frustum[a].normal.z*corner1->z + frustum[a].distance>0)  continue;
		if(frustum[a].normal.x*corner1->x + frustum[a].normal.y*corner2->y + frustum[a].normal.z*corner1->z + frustum[a].distance>0)  continue;
		if(frustum[a].normal.x*corner2->x + frustum[a].normal.y*corner2->y + frustum[a].normal.z*corner1->z + frustum[a].distance>0)  continue;
		if(frustum[a].normal.x*corner1->x + frustum[a].normal.y*corner1->y + frustum[a].normal.z*corner2->z + frustum[a].distance>0)  continue;
		if(frustum[a].normal.x*corner2->x + frustum[a].normal.y*corner1->y + frustum[a].normal.z*corner2->z + frustum[a].distance>0)  continue;
		if(frustum[a].normal.x*corner1->x + frustum[a].normal.y*corner2->y + frustum[a].normal.z*corner2->z + frustum[a].distance>0)  continue;
		if(DotProduct(&frustum[a].normal,corner2)+frustum[a].distance>0)  continue;
		return false;
	}
	return true;
}

int Frustum::cubeTest(Vector* center,float halfWidth)
{
	float xpWidth=center->x+halfWidth;
	float xmWidth=center->x-halfWidth;
	float ypWidth=center->y+halfWidth;
	float ymWidth=center->y-halfWidth;
	float zpWidth=center->z+halfWidth;
	float zmWidth=center->z-halfWidth;
	for(int a=0;a<6;a++)
	{
		if(frustum[a].normal.x*xmWidth+frustum[a].normal.y*ymWidth+frustum[a].normal.z*zmWidth+frustum[a].distance>0)
		   continue;
		if(frustum[a].normal.x*xpWidth+frustum[a].normal.y*ymWidth+frustum[a].normal.z*zmWidth+frustum[a].distance>0)
		   continue;
		if(frustum[a].normal.x*xmWidth+frustum[a].normal.y*ypWidth+frustum[a].normal.z*zmWidth+frustum[a].distance>0)
		   continue;
		if(frustum[a].normal.x*xpWidth+frustum[a].normal.y*ypWidth+frustum[a].normal.z*zmWidth+frustum[a].distance>0)
		   continue;
		if(frustum[a].normal.x*xmWidth+frustum[a].normal.y*ymWidth+frustum[a].normal.z*zpWidth+frustum[a].distance>0)
		   continue;
		if(frustum[a].normal.x*xpWidth+frustum[a].normal.y*ymWidth+frustum[a].normal.z*zpWidth+frustum[a].distance>0)
		   continue;
		if(frustum[a].normal.x*xmWidth+frustum[a].normal.y*ypWidth+frustum[a].normal.z*zpWidth+frustum[a].distance>0)
		   continue;
		if(frustum[a].normal.x*xpWidth+frustum[a].normal.y*ypWidth+frustum[a].normal.z*zpWidth+frustum[a].distance>0)
		   continue;
		return false;
	}
	return true;
}

int Frustum::sphereTest(Vector* center,float radius)
{
	for(int a=0;a<6;a++)
	{
		if(DotProduct(&frustum[a].normal,center)+frustum[a].distance<=-radius)
			return false;
	}
	return true;
}

int Frustum::pointTest(Vector* point)
{
// Basically if the point is behind any of the planes, it can't be in our frustum
// This makes sense.
	for(int a=0;a<6;a++)
	{
// ax+by+cz+d=0 if the point is ON the plane
		if(DotProduct(&frustum[a].normal,point)+frustum[a].distance<=0)
			return false;
	}
	return true;
}

void Frustum::calculate(Camera* cam)
{
	float proj[16],model[16],clip[16];
	float length;

	glGetFloatv(GL_PROJECTION_MATRIX,proj);
	glGetFloatv(GL_MODELVIEW_MATRIX,model);

// Very simple matrix multiplication
// a0  a1  a2  a3
// a4  a5  a6  a7
// a8  a9 a10 a11
//a12 a13 a14 a15
//*
// b0  b1  b2  b3
// b4  b5  b6  b7
// b8  b9 b10 b11
//b12 b13 b14 b15
	clip[0]=model[0]*proj[0]+model[1]*proj[4]+model[2]*proj[8]+model[3]*proj[12];
	clip[1]=model[0]*proj[1]+model[1]*proj[5]+model[2]*proj[9]+model[3]*proj[13];
	clip[2]=model[0]*proj[2]+model[1]*proj[6]+model[2]*proj[10]+model[3]*proj[14];
	clip[3]=model[0]*proj[3]+model[1]*proj[7]+model[2]*proj[11]+model[3]*proj[15];
	clip[4]=model[4]*proj[0]+model[5]*proj[4]+model[6]*proj[8]+model[7]*proj[12];
	clip[5]=model[4]*proj[1]+model[5]*proj[5]+model[6]*proj[9]+model[7]*proj[13];
	clip[6]=model[4]*proj[2]+model[5]*proj[6]+model[6]*proj[10]+model[7]*proj[14];
	clip[7]=model[4]*proj[3]+model[5]*proj[7]+model[6]*proj[11]+model[7]*proj[15];
	clip[8]=model[8]*proj[0]+model[9]*proj[4]+model[10]*proj[8]+model[11]*proj[12];
	clip[9]=model[8]*proj[1]+model[9]*proj[5]+model[10]*proj[9]+model[11]*proj[13];
	clip[10]=model[8]*proj[2]+model[9]*proj[6]+model[10]*proj[10]+model[11]*proj[14];
	clip[11]=model[8]*proj[3]+model[9]*proj[7]+model[10]*proj[11]+model[11]*proj[15];
	clip[12]=model[12]*proj[0]+model[13]*proj[4]+model[14]*proj[8]+model[15]*proj[12];
	clip[13]=model[12]*proj[1]+model[13]*proj[5]+model[14]*proj[9]+model[15]*proj[13];
	clip[14]=model[12]*proj[2]+model[13]*proj[6]+model[14]*proj[10]+model[15]*proj[14];
	clip[15]=model[12]*proj[3]+model[13]*proj[7]+model[14]*proj[11]+model[15]*proj[15];

	frustum[RIGHT].normal.x=clip[3]-clip[0];
	frustum[RIGHT].normal.y=clip[7]-clip[4];
	frustum[RIGHT].normal.z=clip[11]-clip[8];
	frustum[RIGHT].distance=clip[15]-clip[12];
	length=frustum[RIGHT].normal.length();
	frustum[RIGHT].normal/=length;
	frustum[RIGHT].distance/=length;

	frustum[LEFT].normal.x=clip[3]+clip[0];
	frustum[LEFT].normal.y=clip[7]+clip[4];
	frustum[LEFT].normal.z=clip[11]+clip[8];
	frustum[LEFT].distance=clip[15]+clip[12];
	length=frustum[LEFT].normal.length();
	frustum[LEFT].normal/=length;
	frustum[LEFT].distance/=length;

	frustum[BOTTOM].normal.x=clip[3]+clip[1];
	frustum[BOTTOM].normal.y=clip[7]+clip[5];
	frustum[BOTTOM].normal.z=clip[11]+clip[9];
	frustum[BOTTOM].distance=clip[15]+clip[13];
	length=frustum[BOTTOM].normal.length();
	frustum[BOTTOM].normal/=length;
	frustum[BOTTOM].distance/=length;

	frustum[TOP].normal.x=clip[3]-clip[1];
	frustum[TOP].normal.y=clip[7]-clip[5];
	frustum[TOP].normal.z=clip[11]-clip[9];
	frustum[TOP].distance=clip[15]-clip[13];
	length=frustum[TOP].normal.length();
	frustum[TOP].normal/=length;
	frustum[TOP].distance/=length;

// The front plane is basically the normal of the camera frustum at this point
	frustum[FRONT].normal=cam->dir;

// Find the distance by plugging in the plane equation
// Plane equation:
//   ax+by+cz+d=0

// We know the normal a,b,c, plug that in and we get
//  cam->dir.x*x+cam->dir.y*y+cam->dir.z*z+d=0

// We now need to find a point that is ON the plane
// Calculate it by shifting our current position and calculating
//  the view plane by the far plane
	Vector Point=cam->pos+cam->dir*NEAR_PLANE;
// 3 mult, 3 add
// Calculate the dot product between the two vectors
//  EXACT SAME AS PLANE EQUATION!  distance is negative.....
// TODO: Figure out more as to WHY this works
	frustum[FRONT].distance=-DotProduct(&frustum[FRONT].normal,&Point);
// 3 mults, 2 adds
// 6 mults 5 adds


//	PointOnPlane = position+((view)*FAR_PLANE);

// The back plane is the front plane, flipped
	frustum[BACK].normal=cam->dir*-1;
	Point=cam->pos+cam->dir*FAR_PLANE;
	frustum[BACK].distance=-DotProduct(&frustum[BACK].normal,&Point);
//*/




// This is the gametutorials method and involves extracting the front and back
// planes.  This causes sqrt to be called and a lot more operations
/*
	frustum[BACK].normal.x=clip[3]-clip[2];
	frustum[BACK].normal.y=clip[7]-clip[6];
	frustum[BACK].normal.z=clip[11]-clip[10];
	frustum[BACK].distance=clip[15]-clip[14];
// 4 subs
	length=frustum[BACK].normal.length();
// 1 sqrt 3 mults, 2 adds
	frustum[BACK].normal/=length;
	frustum[BACK].distance/=length;
// 4 divs

	frustum[FRONT].normal.x=clip[3]+clip[2];
	frustum[FRONT].normal.y=clip[7]+clip[6];
	frustum[FRONT].normal.z=clip[11]+clip[10];
	frustum[FRONT].distance=clip[15]+clip[14];
	length=frustum[FRONT].normal.length();
	frustum[FRONT].normal/=length;
	frustum[FRONT].distance/=length;//*/

// 1/2 at^2
}