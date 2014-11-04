#ifndef _COLLISIONHANDLER_H
#define _COLLISIONHANDLER_H

#include "stdafx.h"
#include "Map.h"
#include "Vector2.h"
#include "Logger.h"

#define MANAGED_DIVISION_COUNT 3*3
#define MAX_COLLISION_OBJECTS 256

class Object;
struct CollisionDivision
{
   Vector2 min, max;
	Object* objects[MAX_COLLISION_OBJECTS];
	unsigned int objectCount;

	int find(Object* a)
	{
		for (int i = 0; i < objectCount && i < MAX_COLLISION_OBJECTS; i++)
		{
			if (a == objects[i])
				return i;
		}

		return -1;
	}
	
	void clear()
	{
		for (int i = 0; i < objectCount && i < MAX_COLLISION_OBJECTS; i++)
		{
			objects[i] = 0;
		}

		objectCount = 0;
	}

	void push(Object * a)
	{
		objects[objectCount] = a;
		objectCount++;
	}

	void remove(unsigned int i)
	{
		if (i < objectCount)
		{
			objects[i] = 0;
			for (; i!=objectCount; i++)
			{
				objects[i] = (i + 1 >= MAX_COLLISION_OBJECTS) ? (0) : (objects[i + 1]);
			}
			objectCount--;
		}
	}
};

class CollisionHandler
{
public:
   CollisionHandler(Map * map);
   void init(int divisionsOverWidth);
   ~CollisionHandler(){ /*std::free(managedDivisions);*/ }

	void stackChanged(Object *object);// { dirty = true; }
	void redoDatabase();// { dirty = true; }

   void addObject(Object *object);
   void removeObject(Object* object);
   void getDivisions(Object *object, CollisionDivision *divisionArray[], int &a_DivisionCount);

   void update(float a_DT);

private:
   void checkForCollisions(int pos);
   void correctDivisions(int pos);
   void correctUnmanagedDivision();
   void addToDivision(Object* object, int x, int y);
   void addUnmanagedObject(Object* object);
   
   void removeFromDivision(Object* object, int i);
   int width, height;
   CollisionDivision managedDivisions[MANAGED_DIVISION_COUNT];
   CollisionDivision unmanagedDivision;
   int divisionCount;
   Map* chart;
   bool dirty = true;
};

#endif