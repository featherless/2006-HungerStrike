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

#ifndef _VECTOR_H_
#define _VECTOR_H_

// Programmed by Jeff Verkoeyen

#include <math.h>

#define PI 3.14159265358979
#define PI2 6.283185307179

float Fast_sqrt(float num);

template<class T>
T JVMin(T val1,T val2) { if (val1 < val2) return val1; else return val2; }
template<class T>
T JVMax(T val1,T val2) { if (val1 > val2) return val1; else return val2; }

class Vector
{
private:
public:
	double x,y,z;
	Vector() { x=y=z=0; };
	Vector(double X,double Y,double Z) { x=X; y=Y; z=Z; };

	inline bool		operator==(Vector vect)	{ if(x==vect.x && y==vect.y && z==vect.z) return true; else return false; };
	inline Vector	operator- (Vector vect) const	{ return Vector(x-vect.x,y-vect.y,z-vect.z); };
	inline Vector	operator+ (Vector vect) const	{ return Vector(x+vect.x,y+vect.y,z+vect.z); };
	inline void		operator-=(Vector vect)	{ x-=vect.x; y-=vect.y; z-=vect.z; };
	inline void		operator+=(Vector vect)	{ x+=vect.x; y+=vect.y; z+=vect.z; };
	inline Vector	operator- (double mag) const	{ return Vector(x-mag,y-mag,z-mag); };
	inline Vector	operator+ (double mag) const	{ return Vector(x+mag,y+mag,z+mag); };
	inline Vector	operator* (double mag) const	{ return Vector(x*mag,y*mag,z*mag); };
	inline Vector	operator/ (double mag) const	{ return Vector(x/mag,y/mag,z/mag); };
	inline void		operator-=(double mag)	{ x-=mag; y-=mag; z-=mag; };
	inline void		operator+=(double mag)	{ x+=mag; y+=mag; z+=mag; };
	inline void		operator*=(double mag)	{ x*=mag; y*=mag; z*=mag; };
	inline void		operator/=(double mag)	{ x/=mag; y/=mag; z/=mag; };

	double&	operator() (unsigned int index) {	if(index == 0) return x;
                                                if(index == 1) return y;
                                                return z; }

	double length()
	{ return (double)sqrt(x*x+y*y+z*z); }
	double lengthNoSqrt()
	{ return (double)x*x+y*y+z*z; }
	void normalize()
	{ double llength=length(); if(!llength) return; x/=llength; y/=llength; z/=llength; }
	void RotateAxis(double angle,Vector* axis);

	void RenderVectors(double scale,Vector from);
};

class VectorI
{
private:
public:
	int x,y,z;
	VectorI() { x=y=z=0; };
	VectorI(int X,int Y,int Z) { x=X; y=Y; z=Z; };

	Vector GetFloat() { return Vector((double)x,(double)y,(double)z); };
	double length()
	{ return (double)sqrt((double)(x*x+y*y+z*z)); }
	void normalize()
	{ double llength=length(); x=(int)(x/llength); y=(int)(y/llength); z=(int)(z/llength); }
	void RotateAxis(double angle,Vector* axis);
};

class Vector2D
{
private:
public:
	double x,y;
	Vector2D() { x=y=0; };
	Vector2D(double X,double Y) { x=X; y=Y; };

	inline bool		operator==(Vector2D vect)	{ if(x==vect.x && y==vect.y) return true; else return false; };
	inline Vector2D	operator- (Vector2D vect) const { return Vector2D(x-vect.x,y-vect.y); };
	inline Vector2D	operator+ (Vector2D vect) const { return Vector2D(x+vect.x,y+vect.y); };
	inline Vector2D	operator* (Vector2D vect) const { return Vector2D(x*vect.x,y*vect.y); };
	inline Vector2D	operator/ (Vector2D vect) const { return Vector2D(x/vect.x,y/vect.y); };
	inline void		operator-=(Vector2D vect)	{ x-=vect.x; y-=vect.y; };
	inline void		operator+=(Vector2D vect)	{ x+=vect.x; y+=vect.y; };
	inline void		operator*=(Vector2D vect)	{ x*=vect.x; y*=vect.y; };
	inline void		operator/=(Vector2D vect)	{ x/=vect.x; y/=vect.y; };
	inline Vector2D	operator- (double mag) const { return Vector2D(x-mag,y-mag); };
	inline Vector2D	operator+ (double mag) const { return Vector2D(x+mag,y+mag); };
	inline Vector2D	operator* (double mag) const { return Vector2D(x*mag,y*mag); };
	inline Vector2D	operator/ (double mag) const { return Vector2D(x/mag,y/mag); };
	inline void		operator-=(double mag)	{ x-=mag; y-=mag; };
	inline void		operator+=(double mag)	{ x+=mag; y+=mag; };
	inline void		operator*=(double mag)	{ x*=mag; y*=mag; };
	inline void		operator/=(double mag)	{ x/=mag; y/=mag; };

	double&	operator() (unsigned int index) {	if(index == 0) return x;
                                                return y; }

	double length()
	{ return (double)sqrt(x*x+y*y); }
	double lengthNoSqrt()
	{ return (double)x*x+y*y; }
	void normalize()
	{ double llength=length(); x/=llength; y/=llength; }
};

// AKA inner product....
// dotproduct-   a.b=a.x*b.x+a.y*b.y+a.z*b.z
//       also-   a.b=|a||b|cos(theta)
//
//using above- we can find the angle between any two vectors
//
inline double DotProduct(Vector v1,Vector v2)
{ return (v1.x*v2.x+v1.y*v2.y+v1.z*v2.z); }
inline double DotProduct(const Vector* v1,const Vector* v2)
{ return (v1->x*v2->x+v1->y*v2->y+v1->z*v2->z); }

inline double Angle(Vector v1,Vector v2)
{ return (double)acos(DotProduct(v1,v2)/(v1.length()*v2.length())); }
inline double Angle(Vector* v1,Vector* v2)
{ return (double)acos(DotProduct(v1,v2)/(v1->length()*v2->length())); }

inline Vector Normalize(Vector v1)
{ double llength=v1.length();  return Vector(v1.x/llength,v1.y/llength,v1.z/llength); }
inline Vector Normalize(Vector* v1)
{ double llength=v1->length();  return Vector(v1->x/llength,v1->y/llength,v1->z/llength); }



inline Vector CrossProduct(Vector v1,Vector v2)
{ return Vector(v1.y*v2.z-v1.z*v2.y,v1.z*v2.x-v1.x*v2.z,v1.x*v2.y-v1.y*v2.x); }
inline Vector CrossProduct(Vector* v1,Vector* v2)
{ return Vector(v1->y*v2->z-v1->z*v2->y,v1->z*v2->x-v1->x*v2->z,v1->x*v2->y-v1->y*v2->x); }

inline double VectDist(Vector v1,Vector v2)
{ return (double)sqrt((v1.x-v2.x)*(v1.x-v2.x)+
			  (v1.y-v2.y)*(v1.y-v2.y)+
			  (v1.z-v2.z)*(v1.z-v2.z)); }

inline double VectDist(Vector2D v1,Vector2D v2)
{ return (double)sqrt((v1.x-v2.x)*(v1.x-v2.x)+
			  (v1.y-v2.y)*(v1.y-v2.y)); }

inline Vector DirVector(Vector v1,Vector v2)
{ return v1-v2; }

inline Vector TriangleVect(Vector Triangle[])
{
	Vector v1=Triangle[1]-Triangle[0];
	Vector v2=Triangle[2]-Triangle[0];

	Vector normal=CrossProduct(&v1,&v2);
	normal.normalize();
	return normal;
}

class Matrix3X3
{
public:
	friend Matrix3X3 operator*(double c,Matrix3X3 m);

	Matrix3X3(void);
	Matrix3X3(const Matrix3X3* m);

	Matrix3X3 transpose(void) const;
  double det() const;

	Matrix3X3 operator+ (Matrix3X3 m) const;
	Matrix3X3 operator- (Matrix3X3 m) const;
	Matrix3X3 operator* (double c) const;
	Matrix3X3 operator/ (double c) const;
	Matrix3X3 operator* (Matrix3X3 m) const;
	Vector operator* (Vector v) const;

	double&	operator() (unsigned int row, unsigned int col);
	double&	operator() (unsigned int index);
private:
	double matrix[3*3];
};

class Matrix2X2
{
public:
	friend Matrix2X2 operator*(double c,Matrix2X2 m);

	Matrix2X2(void);
	Matrix2X2(const Matrix2X2* m);

	Matrix2X2 transpose(void) const;
	Matrix2X2 inverse(void) const;
  double det() const;
  static Matrix2X2 buildRotation(double dRad);

	Matrix2X2 operator+ (Matrix2X2 m) const;
	Matrix2X2 operator- (Matrix2X2 m) const;
	Matrix2X2 operator* (double c) const;
	Matrix2X2 operator/ (double c) const;
	Matrix2X2 operator* (Matrix2X2 m) const;
	Vector2D operator* (Vector2D v) const;

	double&	operator() (unsigned int row, unsigned int col);
	double&	operator() (unsigned int index);
private:
	double matrix[2*2];
};


class Line
{
public:
	Vector pts[2];
	Line() {};
	Line(Vector pt1,Vector pt2)
	{ pts[0]=pt1; pts[1]=pt2; }

	Vector dir()
	{ return pts[0]-pts[1]; }
};

class Line2D
{
public:
	Vector2D pts[2];
	Line2D() {};
	Line2D(Vector2D pt1,Vector2D pt2)
	{ pts[0]=pt1; pts[1]=pt2; }

	Vector2D dir()
	{ return pts[0]-pts[1]; }
};

enum Positions { BACKSIDE, INTERSECT, FRONTSIDE };

class Plane
{
public:
	Vector normal;
	double distance;

	Plane() {};
	Plane(Vector norm,double dist)
	{ normal=norm; distance=dist; }
	Plane(Vector norm,Vector pointOnPlane)
	{ normal=norm; CalcDistance(&pointOnPlane); }
	Plane(Vector norm,const Vector* pointOnPlane)
	{ normal=norm; CalcDistance(pointOnPlane); }


	void CalcDistance(Vector pointOnPlane)
	{ distance=-DotProduct(&normal,&pointOnPlane); }
	void CalcDistance(const Vector* pointOnPlane)
	{ distance=-DotProduct(&normal,pointOnPlane); }

	double distFromPlane(Vector pt)
	{ return DotProduct(normal,pt)+distance; }

	int lineIntersect(const Line* line,double* dist1,double* dist2)
	{ *dist1=distFromPlane(line->pts[0]); *dist2=distFromPlane(line->pts[1]); return (((*dist1)*(*dist2))<0); }
	int lineIntersect(Line line,double* dist1,double* dist2)
	{ *dist1=distFromPlane(line.pts[0]); *dist2=distFromPlane(line.pts[1]); return (((*dist1)*(*dist2))<0); }

	int lineIntersect(const Vector line[],double* dist1,double* dist2)
	{ *dist1=distFromPlane(line[0]); *dist2=distFromPlane(line[1]); return (((*dist1)*(*dist2))<0); }
	int lineIntersect(Vector pt1,Vector pt2,double* dist1,double* dist2)
	{ *dist1=distFromPlane(pt1); *dist2=distFromPlane(pt2); return (((*dist1)*(*dist2))<0); }

// If you don't need the distance of intersection
	int lineIntersect(const Line* line)
	{ return ((distFromPlane(line->pts[0])*distFromPlane(line->pts[1]))<0); }
	int lineIntersect(Line line)
	{ return ((distFromPlane(line.pts[0])*distFromPlane(line.pts[1]))<0); }

	int lineIntersect(Vector line[])
	{ return ((distFromPlane(line[0])*distFromPlane(line[1]))<0); }
	int lineIntersect(Vector pt1,Vector pt2)
	{ return ((distFromPlane(pt1)*distFromPlane(pt2))<0); }

// Takes a line and calculates its direction (a unit vector parallel to the line)
//  it then it projects the normal of the plane on to the line
	Vector PointOfIntersection(Line* line,double dist1)
	{
		Vector LineDir=line->dir();
		double dist2=DotProduct(normal,LineDir);
// dist2 is the length of the line

// if dist2 is 0, then this line has no length and must reside on the plane
		if(!dist2)	return line->pts[0];

// Calculate the offset distance by doing a percentage (distance to the intersection divided by
//  the distance from point a to point b)
		double dist=-dist1/dist2;

// Project this point along the line
		return line->pts[0]+LineDir*dist;
	}

	int SpherePosition(const Vector* center,double radius,double* dist)
	{
		*dist=distFromPlane(*center);

		if(fabs(*dist)<radius)	return INTERSECT;
		else if(*dist>=radius)	return FRONTSIDE;

		return BACKSIDE;
	}

	int TestSpherecollision(const Vector* center,double radius)
	{
		double dist=0.0f;
		int type=SpherePosition(center,radius,&dist);
		if(type!=INTERSECT)		// No collision!
			return 0;

		return 1;
	}
};

class Triangle
{
public:
	Vector pts[3];
	Plane plane;

	Triangle() {};
	Triangle(Vector pt1,Vector pt2,Vector pt3)
	{ pts[0]=pt1; pts[1]=pt2; pts[2]=pt3; }

	void CalcPlane()
	{ Vector normal=CrossProduct(pts[2]-pts[0],pts[1]-pts[0]); normal.normalize(); plane=Plane(normal,&pts[0]); }

	int PointInPlane(const Vector* pt)
	{
		double degrees=0.0f;
		for(int a=0;a<3;a++)
			degrees+=Angle(pts[a]-*pt,pts[(a+1)%3]-*pt);
		if(degrees>=(2*PI*0.999f))
			return true;
		else return false;
	}

	double HeightAtPos(double x,double z)
	{
		return (-plane.distance-plane.normal.x*x-plane.normal.z*z)/plane.normal.y;
	}

	int TestSpherecollision(Vector* center,double radius,Vector* collisionPoint)
	{
		double dist=0.0f;
		int type=plane.SpherePosition(center,radius,&dist);
		if(type!=INTERSECT)		// No collision!
			return 0;

// Project the sphere on to the plane
		*collisionPoint=*center-plane.normal*dist;

		if(PointInPlane(collisionPoint))
			return 1;

		return 0;
	}
};

class Rectangle2D
{
public:
  Rectangle2D() { m_dRotation = 0; };
  Rectangle2D(Vector2D ptTopLeft,Vector2D ptBottomRight)
  {
    pts[0] = vectTopLeft = ptTopLeft;
    pts[1] = vectTopRight = Vector2D(ptBottomRight.x,ptTopLeft.y);
    pts[2] = vectBottomRight = ptBottomRight;
    pts[3] = vectBottomLeft = Vector2D(ptTopLeft.x,ptBottomRight.y);
    m_dRotation = 0;
  }
  Rectangle2D(Vector2D* pptTopLeft,Vector2D* pptBottomRight)
  {
    pts[0] = vectTopLeft = *pptTopLeft;
    pts[1] = vectTopRight = Vector2D(pptBottomRight->x,pptTopLeft->y);
    pts[2] = vectBottomRight = *pptBottomRight;
    pts[3] = vectBottomLeft = Vector2D(pptTopLeft->x,pptBottomRight->y);
    m_dRotation = 0;
  }
  Rectangle2D(Vector2D ptTopLeft,Vector2D ptTopRight,Vector2D ptBottomRight,Vector2D ptBottomLeft)
  {
    pts[0] = vectTopLeft = ptTopLeft;
    pts[1] = vectTopRight = ptTopRight;
    pts[2] = vectBottomRight = ptBottomRight;
    pts[3] = vectBottomLeft = ptBottomLeft;
    m_dRotation = 0;
  }
  Rectangle2D(Vector2D *pptTopLeft,Vector2D *pptTopRight,Vector2D *pptBottomRight,Vector2D *pptBottomLeft)
  {
    pts[0] = vectTopLeft = *pptTopLeft;
    pts[1] = vectTopRight = *pptTopRight;
    pts[2] = vectBottomRight = *pptBottomRight;
    pts[3] = vectBottomLeft = *pptBottomLeft;
    m_dRotation = 0;
  }

  double width()
  {
    return VectDist(vectTopLeft,vectTopRight);
  }
  double height()
  {
    return VectDist(vectTopLeft,vectBottomLeft);
  }
  double rotation()
  {
    return m_dRotation;
  }

  void Rotate(double dRadians)
  {
    m_dRotation += dRadians;
    if (m_dRotation >= PI2)
      m_dRotation -= PI2;
    else if (m_dRotation < 0)
      m_dRotation += PI2;

    m_matRotation = Matrix2X2::buildRotation(m_dRotation);
    vectTopLeft = m_matRotation * pts[0];
    vectTopRight = m_matRotation * pts[1];
    vectBottomRight = m_matRotation * pts[2];
    vectBottomLeft = m_matRotation * pts[3];
  }

  void Scale(double dScale)
  {
    vectTopLeft *= dScale;
    vectTopRight *= dScale;
    vectBottomRight *= dScale;
    vectBottomLeft *= dScale;
  }

  Rectangle2D CalculateBoundingRect()
  {
    Rectangle2D BoundingRect;
    BoundingRect.vectTopLeft = Vector2D(JVMin(JVMin(vectTopLeft.x,vectTopRight.x),JVMin(vectBottomRight.x,vectBottomLeft.x)),
                                        JVMax(JVMax(vectTopLeft.y,vectTopRight.y),JVMax(vectBottomRight.y,vectBottomLeft.y)));
    BoundingRect.vectTopRight = Vector2D(JVMax(JVMax(vectTopLeft.x,vectTopRight.x),JVMax(vectBottomRight.x,vectBottomLeft.x)),
                                        JVMax(JVMax(vectTopLeft.y,vectTopRight.y),JVMax(vectBottomRight.y,vectBottomLeft.y)));
    BoundingRect.vectBottomRight = Vector2D(JVMax(JVMax(vectTopLeft.x,vectTopRight.x),JVMax(vectBottomRight.x,vectBottomLeft.x)),
                                        JVMin(JVMin(vectTopLeft.y,vectTopRight.y),JVMin(vectBottomRight.y,vectBottomLeft.y)));
    BoundingRect.vectBottomLeft = Vector2D(JVMin(JVMin(vectTopLeft.x,vectTopRight.x),JVMin(vectBottomRight.x,vectBottomLeft.x)),
                                        JVMin(JVMin(vectTopLeft.y,vectTopRight.y),JVMin(vectBottomRight.y,vectBottomLeft.y)));

    return BoundingRect;
  }

  Vector2D GetTopLeft() { return vectTopLeft; }
  Vector2D GetTopRight() { return vectTopRight; }
  Vector2D GetBottomRight() { return vectBottomRight; }
  Vector2D GetBottomLeft() { return vectBottomLeft; }
private:
	Vector2D pts[4];
  Vector2D vectTopLeft;
  Vector2D vectTopRight;
  Vector2D vectBottomRight;
  Vector2D vectBottomLeft;

  Matrix2X2 m_matRotation;
  double m_dRotation;
};

#endif
