#include "CollisionHandler.h"
#include "Map.h"
#include "AIMesh.h"
#include "Unit.h"
#include "Pathfinder.h"
#include "Minion.h"
#include "Champion.h"
#include "Logger.h"
#include "Object.h"
#include <iostream>

CollisionHandler::CollisionHandler(Map*map) : chart(map)
{ 
   Pathfinder::setMap(map);
   divisionCount = -1;
}

void CollisionHandler::init(int divisionsOverWidth)
{
   width = chart->getAIMesh()->getWidth();
   height = chart->getAIMesh()->getHeight();

   //divisionCount = divisionsOverWidth;
   divisionCount = sqrt(MANAGED_DIVISION_COUNT);

   //managedDivisions = (CollisionDivision*)std::malloc(sizeof(CollisionDivision)*divisionsOverWidth*divisionsOverWidth);
   for (int y = 0; y < divisionsOverWidth; y++)
   {
      for (int x = 0; x < divisionsOverWidth; x++)
      {
         managedDivisions[y*divisionsOverWidth + x].min.X = x*(width / divisionsOverWidth);
         managedDivisions[y*divisionsOverWidth + x].min.Y = y*(height / divisionsOverWidth);

         managedDivisions[y*divisionsOverWidth + x].max.X = (x + 1)*(width / divisionsOverWidth);
         managedDivisions[y*divisionsOverWidth + x].max.Y = (y + 1)*(height / divisionsOverWidth);
      }
   }
}

void CollisionHandler::checkForCollisions(int pos)
{
   auto curDiv = managedDivisions[pos];
   for (int i = 0; i < curDiv.objects.size(); i++)
   {
      auto o1 = curDiv.objects.at(i);
      if (abs(o1->getPosition().X) < 0.0001f && abs(o1->getPosition().Y) < 0.0001f)
      {
         removeObject(o1);
         continue;
      }

      for (int j = 0; j < curDiv.objects.size(); j++) if (j != i)
      {
         auto o2 = curDiv.objects.at(j);
         if (abs(o2->getPosition().X) < 0.0001f && abs(o2->getPosition().Y) < 0.0001f)
         {
            removeObject(o2);
            continue;
         }

         auto displ = (o2->getPosition() - o1->getPosition());
         if (displ.SqrLength() < (o1->getCollisionRadius() + o2->getCollisionRadius())*(o1->getCollisionRadius() + o2->getCollisionRadius()))
         {
            o1->onCollision(o2);
            //o2->onCollision(o1); // Is being done by the second iteration.
         }  
      }
   }
}

void CollisionHandler::update(float deltatime)
{
   //correctUnmanagedDivision();

   for (int i = 0; i < divisionCount*divisionCount; i++)
   {
      checkForCollisions(i);
      correctDivisions(i);
   }
}

void CollisionHandler::correctDivisions(int pos)
{
   CollisionDivision curDiv = managedDivisions[pos];
   for (int j = 0; j < curDiv.objects.size(); j++)
   {
      Object* o = (curDiv.objects.at(j));

      if (abs(o->getPosition().X) < 0.0001f && abs(o->getPosition().Y) < 0.0001f)
      {
         removeObject(o);
         continue;
      }

      Vector2 center = curDiv.min + ((curDiv.max - curDiv.min)*0.5f);

      //if (o->moved)
      {
         if ((o->getPosition().X - o->getCollisionRadius() > curDiv.max.X || o->getPosition().Y - o->getCollisionRadius() > curDiv.max.Y ||
            o->getPosition().X + o->getCollisionRadius() < curDiv.min.X || o->getPosition().Y + o->getCollisionRadius() < curDiv.min.Y))
         {
            removeFromDivision(o, pos);
            addObject(o);
         }
         else if ((o->getPosition().X + o->getCollisionRadius() > curDiv.max.X || o->getPosition().Y + o->getCollisionRadius() > curDiv.max.Y ||
            o->getPosition().X - o->getCollisionRadius() < curDiv.min.X || o->getPosition().Y - o->getCollisionRadius() < curDiv.min.Y))
         {
            addObject(o);
         }
      }
   }
}

void CollisionHandler::correctUnmanagedDivision()
{
   CollisionDivision curDiv = unmanagedDivision;
   for (int j = 0; j < curDiv.objects.size(); j++)
   {
      Object* o = (curDiv.objects.at(j));

      Vector2 center = curDiv.min + ((curDiv.max - curDiv.min)*0.5f);

      if (o->isMovementUpdated())
      {
         if ((o->getPosition().X - o->getCollisionRadius() > width || o->getPosition().Y - o->getCollisionRadius() > height ||
            o->getPosition().X + o->getCollisionRadius() < 0 || o->getPosition().Y + o->getCollisionRadius() < 0))
         {
            removeFromDivision(o, -1);
            addObject(o);
         }
      }
   }
}

void CollisionHandler::addObject(Object *object)
{
   if (dynamic_cast<Minion*>(object) == 0 && dynamic_cast<Champion*>(object) == 0)//&& dynamic_cast<Turret*>(object) == 0 && dynamic_cast<LevelProp*>(object) == 0)
      return;

   if (divisionCount == -1)
   {
      //printf("Added an object before we initialised the CollisionHandler!");
      //addUnmanagedObject(object);
   }

   float divX = object->getPosition().X / (float)(width / divisionCount);
   float divY = object->getPosition().Y / (float)(height / divisionCount);

   int divi = (int)divY*divisionCount + (int)divX;

   if (divX < 0 || divX > divisionCount || divY < 0 || divY > divisionCount)
   {
      //printf("Object spawned outside of map. (%f, %f)", object->getPosition().X, object->getPosition().Y);
      //addUnmanagedObject(object);
   }
   else
   {
      addToDivision(object, (int)divX, (int)divY);
      CollisionDivision curDiv = managedDivisions[divi];

      bool a = false, b = false;
      if (abs(object->getPosition().X - curDiv.max.X) < object->getCollisionRadius())
         addToDivision(object, (int)divX + 1, (int)divY);
      if (abs(object->getPosition().X - curDiv.min.X) < object->getCollisionRadius())
         a = true, addToDivision(object, (int)divX - 1, (int)divY);

      if (abs(object->getPosition().Y - curDiv.max.Y) < object->getCollisionRadius())
         addToDivision(object, (int)divX, (int)divY + 1);
      if (abs(object->getPosition().Y - curDiv.min.Y) < object->getCollisionRadius())
         b = true, addToDivision(object, (int)divX, (int)divY - 1);

      if (a && b)
         b = true, addToDivision(object, (int)divX - 1, (int)divY - 1);
   }
}

void CollisionHandler::getDivisions(Object *object, CollisionDivision *divs[], int &divCount)
{
   for (int i = 0; i < 4; i++)
   {
      divs[i] = 0;
   }

   divCount = 0;
   float divX = object->getPosition().X / (float)(width / divisionCount);
   float divY = object->getPosition().Y / (float)(height / divisionCount);

   int divi = (int)divY*divisionCount + (int)divX;

   if (divY >= 0 && divY < divisionCount)
   {
      divs[divCount] = &managedDivisions[divi];
      divCount++;
   }

   bool a = false, b = false;
   auto curDiv = managedDivisions[divi];
   if (abs(object->getPosition().X - curDiv.max.X) < object->getCollisionRadius() && divX + 1 >= 0 && divX + 1 < divisionCount)
   {
      divs[divCount] = &managedDivisions[(int)divY*divisionCount + (int)divX + 1];
      divCount++;
   }
   else if (abs(object->getPosition().X - curDiv.min.X) < object->getCollisionRadius() && divX - 1 >= 0 && divX - 1 < divisionCount)
   {
      divs[divCount] = &managedDivisions[(int)divY*divisionCount + (int)divX - 1];
      divCount++;
      a = true;
   }
   if (abs(object->getPosition().Y - curDiv.max.Y) < object->getCollisionRadius() && divY + 1 >= 0 && divY + 1 < divisionCount)
   {
      divs[divCount] = &managedDivisions[(int)divY*divisionCount + (int)divX + 1];
      divCount++;
   }
   else if (abs(object->getPosition().Y - curDiv.min.Y) < object->getCollisionRadius() && divY - 1 >= 0 && divY - 1 < divisionCount)
   {
      divs[divCount] = &managedDivisions[(int)divY*divisionCount + (int)divX + 1];
      divCount++;
      b = true;
   }

   if (a && b && divX + 1 >= 0 && divX + 1 < divisionCount)
   {
      divs[divCount] = &managedDivisions[(int)divY*divisionCount + (int)divX + 1];
      divCount++;
   }
}

void CollisionHandler::addToDivision(Object* object, int x, int y)
{
   if (y >= 0 && y < divisionCount && x >= 0 && x < divisionCount)
   {
      int pos = y*divisionCount + x;
      if (std::find(managedDivisions[pos].objects.begin(), managedDivisions[pos].objects.end(), object) == managedDivisions[pos].objects.end())
      {
         managedDivisions[pos].objects.push_back(object);
      }
   }
}

void CollisionHandler::addUnmanagedObject(Object* object)
{
   //if(y < 0 || y >= divisionCount || x < 0 || x >= divisionCount))
   {
      if (std::find(unmanagedDivision.objects.begin(), unmanagedDivision.objects.end(), object) == unmanagedDivision.objects.end())
      {
         unmanagedDivision.objects.push_back(object);
      }
   }
}


void CollisionHandler::removeObject(Object* object)
{
   CollisionDivision * curDiv;
   for (int i = -1; i < divisionCount; i++)
   {
      if (i == -1) curDiv = &unmanagedDivision;
      else curDiv = &managedDivisions[i];

      auto j = std::find(curDiv->objects.begin(), curDiv->objects.end(), object);
      while (j != curDiv->objects.end())
      {
         curDiv->objects.erase(j);
         j = std::find(curDiv->objects.begin(), curDiv->objects.end(), object);
      }
   }
}

void CollisionHandler::removeFromDivision(Object* object, int i)
{
   CollisionDivision * curDiv;
   if (i == -1) curDiv = &unmanagedDivision;
   else curDiv = &managedDivisions[i];

   auto j = std::find(curDiv->objects.begin(), curDiv->objects.end(), object);
   while (j != curDiv->objects.end())
   {
      curDiv->objects.erase(j);
      j = std::find(curDiv->objects.begin(), curDiv->objects.end(), object);
   }
}