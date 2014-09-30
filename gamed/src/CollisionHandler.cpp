#include "CollisionHandler.h"
#include "Map.h"
#include <chrono>
#include <math.h>

void CollisionHandler::update(float a_DT)
{
	const std::map<uint32, Object*>& objects = chart->getObjects();

	for (auto i = objects.begin(); i != objects.end(); i++)
	{
		Object* o1 = i->second;
      for (auto j = objects.begin(); j != objects.end(); j++) if (j != i)
      {
         Object* o2 = j->second;

         if ((o1->getPosition() - o2->getPosition()).SqrLength() < (o1->getCollisionRadius() + o2->getCollisionRadius())*(o1->getCollisionRadius() + o2->getCollisionRadius()))
         {
            o1->onCollision(o2);
            //o2->onCollision(01); 
            // Don't do this, it's being done in a later iteration, given that o1 doesn't move away in onCollision.
            // Uh, if it does, uncomment it I guess.
         }
		}
	}
}