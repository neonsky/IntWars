#include "CollisionHandler.h"
#include "Map.h"
#include <chrono>
#include <math.h>

#define COLLISION_BOX_SIZE 50.0f

CollisionHandler::CollisionHandler()
{
	m_Width = ceil((float)MAP_WIDTH / (0.5f*COLLISION_BOX_SIZE)); //  * 2 / 50
	m_Height = ceil((float)MAP_HEIGHT / (0.5f*COLLISION_BOX_SIZE));  // Because for some reason the map width/h is half the width/h of the map?

	printf("Spawned collision map (%d, %d)\n", m_Width, m_Height);
	m_CollisionData = new Object*[m_Width*m_Height];
}

CollisionHandler::~CollisionHandler()
{
	delete[] m_CollisionData;
}

void CollisionHandler::update(float a_DT)
{
	for (int i = 0; i < m_Height*m_Width; i++)
		m_CollisionData[i] = NULL; // Clear the collision data

	const std::map<uint32, Object*>& t_Objects = m_Map->getObjects();

	for (auto i = t_Objects.begin(); i != t_Objects.end(); i++)
	{
		Object* t_Object1 = i->second;

		float x = t_Object1->getX();
		float y = t_Object1->getY();
		Object* t_Target = getCollisionData(t_Object1->getPosition());

		//printf("Object at (%f,%f) (%f,%f)\n", t_Object1->getX(), t_Object1->getY());

		if(t_Target!=NULL) // Is collision occupied
		{
			t_Object1->onCollision(t_Target);
         t_Target->onCollision(t_Object1);
		}
		else
		{
			setCollisionData(t_Object1);
		}
	}
}

void CollisionHandler::setCollisionData(Object* a_Object)
{
	int32 t_X = a_Object->getX() / COLLISION_BOX_SIZE;
	int32 t_Y = a_Object->getY() / COLLISION_BOX_SIZE;

	if (t_X >= 0 && t_Y >= 0 && t_X < m_Width && t_Y < m_Height)
		m_CollisionData[t_Y + t_X*m_Height] = a_Object;
	//else printf("Set out of bounds location: x=%d y=%d, x=%f y=%f\n", t_X, t_Y, a_Object->getX(), a_Object->getY());
}

Object* CollisionHandler::getCollisionData(Vector2 a_Target)
{
   int32 t_X = a_Target.X / COLLISION_BOX_SIZE;
   int32 t_Y = a_Target.Y / COLLISION_BOX_SIZE;

   if (t_X >= 0 && t_Y >= 0 && t_X < m_Width && t_Y < m_Height)
      return m_CollisionData[t_Y + t_X*m_Height];
	
   //printf("Get out of bounds location: x=%d y=%d\n", t_X, t_Y);
   return 0;
}