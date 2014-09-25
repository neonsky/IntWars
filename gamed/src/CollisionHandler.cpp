#include "CollisionHandler.h"
#include "Map.h"
#include <chrono>
#include <math.h>

#define COLLISION_BOX_SIZE 50.0f

CollisionHandler::CollisionHandler()
{
	width = ceil((float)MAP_WIDTH / (0.5f*COLLISION_BOX_SIZE)); //  * 2 / 50
	height = ceil((float)MAP_HEIGHT / (0.5f*COLLISION_BOX_SIZE));  // Because for some reason the map width/h is half the width/h of the map?

	printf("Spawned collision map (%d, %d)\n", width, height);
	collisionData = new Object*[width*height];
}

CollisionHandler::~CollisionHandler()
{
	delete[] collisionData;
}

void CollisionHandler::update(float a_DT)
{
	for (int i = 0; i < height*width; i++)
		collisionData[i] = NULL; // Clear the collision data

	const std::map<uint32, Object*>& objects = m_Map->getObjects();

	for (auto i = objects.begin(); i != objects.end(); i++)
	{
		Object* tempObject = i->second;

		float x = tempObject->getX();
		float y = tempObject->getY();
		Object* target = getCollisionData(tempObject->getPosition());

		//printf("Object at (%f,%f) (%f,%f)\n", tempObject->getX(), tempObject->getY());

		if(target!=NULL) // Is collision occupied
		{
			tempObject->onCollision(target);
         target->onCollision(tempObject);
		}
		else
		{
			setCollisionData(tempObject);
		}
	}
}

void CollisionHandler::setCollisionData(Object* a_Object)
{
	int32 x = a_Object->getX() / COLLISION_BOX_SIZE;
	int32 y = a_Object->getY() / COLLISION_BOX_SIZE;

	if (x >= 0 && y >= 0 && x < width && y < height)
		collisionData[y + x*height] = a_Object;
	//else printf("Set out of bounds location: x=%d y=%d, x=%f y=%f\n", x, y, a_Object->getX(), a_Object->getY());
}

Object* CollisionHandler::getCollisionData(Vector2 a_Target)
{
   int32 x = a_Target.X / COLLISION_BOX_SIZE;
   int32 y = a_Target.Y / COLLISION_BOX_SIZE;

   if (x >= 0 && y >= 0 && x < width && y < height)
      return collisionData[y + x*height];
	
   //printf("Get out of bounds location: x=%d y=%d\n", x, y);
   return 0;
}