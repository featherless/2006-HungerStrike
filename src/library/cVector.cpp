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

#include "cVector.h"

// Programmed by Jeff Verkoeyen

#pragma warning(disable : 4514)

void Vector::RotateAxis(double angle,Vector* axis)
{/*
	double fLength=this->length();
	double sinTheta=sinf(angle);

	*this+=CrossProduct(*this,*axis)*sinTheta;
	this->normalize();
	*this*=fLength;*/

	double fLength=this->length();
	double cosTheta = (double)cos(angle);
	double sinTheta = (double)sin(angle);

	double x,y,z;
	x  = (cosTheta + (1 - cosTheta) * axis->x * axis->x)			* this->x;
	x += ((1 - cosTheta) * axis->x * axis->y - axis->z * sinTheta)	* this->y;
	x += ((1 - cosTheta) * axis->x * axis->z + axis->y * sinTheta)	* this->z;

	y  = ((1 - cosTheta) * axis->x * axis->y + axis->z * sinTheta)	* this->x;
	y += (cosTheta + (1 - cosTheta) * axis->y * axis->y)			* this->y;
	y += ((1 - cosTheta) * axis->y * axis->z - axis->x * sinTheta)	* this->z;

	z  = ((1 - cosTheta) * axis->x * axis->z - axis->y * sinTheta)	* this->x;
	z += ((1 - cosTheta) * axis->y * axis->z + axis->x * sinTheta)	* this->y;
	z += (cosTheta + (1 - cosTheta) * axis->z * axis->z)			* this->z;

	this->x=x;
	this->y=y;
	this->z=z;

	this->normalize();
	*this*=fLength;
}

float Fast_sqrt(float num)
{
	float result;
	_asm
	{
		mov eax, num
		sub eax, 0x3f800000
		sar eax, 1
		add eax, 0x3f800000
		mov result, eax
	}
	return result;
}