

#ifndef VECTOR2_H
#define	VECTOR2_H


#pragma once

#include <math.h>

struct MovementVector;
class Vector2
{
public:
   Vector2() : Vector2(0, 0) { }
   Vector2(float x, float y) : x(x), y(y) { }
   Vector2(const Vector2& v) : x(v.x), y(v.y) { }
   ~Vector2(void);
   float Length();
   float SqrLength();
   Vector2 Normalize();
   float x, y;

	operator MovementVector();

   void operator += (const Vector2& a_V) { x += a_V.x; y += a_V.y; }
   void operator += (Vector2* a_V) { x += a_V->x; y += a_V->y; }
   void operator -= (const Vector2& a_V) { x -= a_V.x; y -= a_V.y; }
   void operator -= (Vector2* a_V) { x -= a_V->x; y -= a_V->y; }
   void operator *= (const float f) { x *= f; y *= f;  }
   void operator *= (const Vector2& a_V) { x *= a_V.x; y *= a_V.y; }
   void operator *= (Vector2* a_V) { x *= a_V->x; y *= a_V->y; }

   Vector2 operator- () const { return Vector2(-x, -y); }
   friend Vector2 operator + (const Vector2& v1, const Vector2& v2) { return Vector2(v1.x + v2.x, v1.y + v2.y); }
   friend Vector2 operator + (const Vector2& v1, Vector2* v2) { return Vector2(v1.x + v2->x, v1.y + v2->y); }
   friend Vector2 operator - (const Vector2& v1, const Vector2& v2) { return Vector2(v1.x - v2.x, v1.y - v2.y); }
   friend Vector2 operator - (const Vector2& v1, Vector2* v2) { return Vector2(v1.x - v2->x, v1.y - v2->y); }
   friend Vector2 operator - (const Vector2* v1, Vector2& v2) { return Vector2(v1->x - v2.x, v1->y - v2.y); }
   // friend Vector2 operator - ( const Vector2* v1, Vector2* v2 ) { return Vector2( v1->x - v2->x, v1->y - v2->y, v1->z - v2->z ); }
   friend Vector2 operator * (const Vector2& v, const float f) { return Vector2(v.x * f, v.y * f); }
   friend Vector2 operator * (const Vector2& v1, const Vector2& v2) { return Vector2(v1.x * v2.x, v1.y * v2.y); }
   friend Vector2 operator * (const float f, const Vector2& v) { return Vector2(v.x * f, v.y * f); }
   friend Vector2 operator / (const Vector2& v, const float f) { return Vector2(v.x / f, v.y / f); }
   friend Vector2 operator / (const Vector2& v1, const Vector2& v2) { return Vector2(v1.x / v2.x, v1.y / v2.y); }
   friend Vector2 operator / (const float f, const Vector2& v) { return Vector2(v.x / f, v.y / f); }

};
#endif	/* VECTOR2_H */

