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
      for (auto j = objects.begin(); j != objects.end(); j++) if (j != i) // Object-Object collision
      {
         Object* o2 = j->second;

         if ((o1->getPosition() - o2->getPosition()).SqrLength() < (o1->getCollisionRadius() + o2->getCollisionRadius())*(o1->getCollisionRadius() + o2->getCollisionRadius())) //distance check
         {
            o1->onCollision(o2); // Call the collision callback
            //o2->onCollision(01); 
            // Don't do this, it's being done in a later iteration, given that o1 doesn't move away in onCollision.
            // Uh, if it does, uncomment it I guess.
         }
		}

      Vector2 direction = (o1->getTarget()->getPosition() - o1->getPosition()).Normalize(); // Get the direction where I am walking into
      direction = direction * o1->getCollisionRadius(); // Multiply it with how fat I am
      
      direction = o1->getPosition() + direction;
      if (mesh->getY(direction.X, direction.Y) == 0.0f)
         o1->onCollision(0);

	}
}