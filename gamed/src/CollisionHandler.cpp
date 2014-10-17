#include "CollisionHandler.h"
#include "Map.h"
#include "AIMesh.h"
#include "Unit.h"
#include "Pathfinder.h"
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
      for (int j = 0; j < curDiv.objects.size(); j++) if (j != i)
      {
         auto o2 = curDiv.objects.at(j);
         auto displ = (o2->getPosition() - o1->getPosition());
         if (displ.SqrLength() < (o1->getLargestRadius() + o2->getLargestRadius())*(o1->getLargestRadius() + o2->getLargestRadius()))
         {
            o1->onCollision(o2);
            //o2->onCollision(o1); // Is being done by the second iteration.
         }
      }
   }
}

void CollisionHandler::update(float deltatime)
{
   correctUnmanagedDivision();

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

      Vector2 center = curDiv.min + ((curDiv.max - curDiv.min)*0.5f);

      //if (o->moved)
      {
         if ((o->getPosition().X - o->getLargestRadius() > curDiv.max.X || o->getPosition().Y - o->getLargestRadius() > curDiv.max.Y ||
            o->getPosition().X + o->getLargestRadius() < curDiv.min.X || o->getPosition().Y + o->getLargestRadius() < curDiv.min.Y))
         {
            removeFromDivision(o, pos);
            addObject(o);
         }
         else if ((o->getPosition().X + o->getLargestRadius() > curDiv.max.X || o->getPosition().Y + o->getLargestRadius() > curDiv.max.Y ||
            o->getPosition().X - o->getLargestRadius() < curDiv.min.X || o->getPosition().Y - o->getLargestRadius() < curDiv.min.Y))
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

      //if (o->moved)
      {
         if ((o->getPosition().X - o->getLargestRadius() > width || o->getPosition().Y - o->getLargestRadius() > height ||
            o->getPosition().X + o->getLargestRadius() < 0 || o->getPosition().Y + o->getLargestRadius() < 0))
         {
            removeFromDivision(o, -1);
            addObject(o);
         }
      }
   }
}

void CollisionHandler::addObject(Object *a_Object)
{
   if (divisionCount == -1)
   {
      CORE_WARNING("Added an object before we initialised!");
      addUnmanagedObject(a_Object);
   }

   float divX = a_Object->getPosition().X / (float)(width / divisionCount);
   float divY = a_Object->getPosition().Y / (float)(height / divisionCount);

   int divi = (int)divY*divisionCount + (int)divX;

   if (divX < 0 || divX > divisionCount || divY < 0 || divY > divisionCount)
   {
      CORE_WARNING("Object spawned outside of map.");
      addUnmanagedObject(a_Object);
   }
   else
   {
      addToDivision(a_Object, (int)divX, (int)divY);
      CollisionDivision curDiv = managedDivisions[divi];

      bool a = false, b = false;
      if (abs(a_Object->getPosition().X - curDiv.max.X) < a_Object->getLargestRadius())
         addToDivision(a_Object, (int)divX + 1, (int)divY);
      if (abs(a_Object->getPosition().X - curDiv.min.X) < a_Object->getLargestRadius())
         a = true, addToDivision(a_Object, (int)divX - 1, (int)divY);

      if (abs(a_Object->getPosition().Y - curDiv.max.Y) < a_Object->getLargestRadius())
         addToDivision(a_Object, (int)divX, (int)divY + 1);
      if (abs(a_Object->getPosition().Y - curDiv.min.Y) < a_Object->getLargestRadius())
         b = true, addToDivision(a_Object, (int)divX, (int)divY - 1);

      if (a && b)
         b = true, addToDivision(a_Object, (int)divX - 1, (int)divY - 1);
   }
}

void CollisionHandler::getDivisions(Object *a_Object, CollisionDivision *a_Divisions[], int &a_DivisionCount)
{
   for (int i = 0; i < 4; i++)
   {
      a_Divisions[i] = 0;
   }

   int currentDiv = 0;
   float divX = a_Object->getPosition().X / (float)(width / divisionCount);
   float divY = a_Object->getPosition().Y / (float)(height / divisionCount);

   int divi = (int)divY*divisionCount + (int)divX;

   if (divY >= 0 && divY < divisionCount)
   {
      a_Divisions[currentDiv] = &managedDivisions[divi];
      currentDiv++;
   }

   bool a = false, b = false;
   auto curDiv = managedDivisions[divi];
   if (abs(a_Object->getPosition().X - curDiv.max.X) < a_Object->getLargestRadius() && divX + 1 >= 0 && divX + 1 < divisionCount)
   {
      a_Divisions[currentDiv] = &managedDivisions[(int)divY*divisionCount + (int)divX + 1];
      currentDiv++;
   }
   else if (abs(a_Object->getPosition().X - curDiv.min.X) < a_Object->getLargestRadius() && divX - 1 >= 0 && divX - 1 < divisionCount)
   {
      a_Divisions[currentDiv] = &managedDivisions[(int)divY*divisionCount + (int)divX - 1];
      currentDiv++;
      a = true;
   }
   if (abs(a_Object->getPosition().Y - curDiv.max.Y) < a_Object->getLargestRadius() && divY + 1 >= 0 && divY + 1 < divisionCount)
   {
      a_Divisions[currentDiv] = &managedDivisions[(int)divY*divisionCount + (int)divX + 1];
      currentDiv++;
   }
   else if (abs(a_Object->getPosition().Y - curDiv.min.Y) < a_Object->getLargestRadius() && divY - 1 >= 0 && divY - 1 < divisionCount)
   {
      a_Divisions[currentDiv] = &managedDivisions[(int)divY*divisionCount + (int)divX + 1];
      currentDiv++;
      b = true;
   }

   if (a && b && divX + 1 >= 0 && divX + 1 < divisionCount)
   {
      a_Divisions[currentDiv] = &managedDivisions[(int)divY*divisionCount + (int)divX + 1];
      currentDiv++;
   }
}

void CollisionHandler::addToDivision(Object* a_Object, int x, int y)
{
   if (y >= 0 && y < divisionCount && x >= 0 && x < divisionCount)
   {
      int pos = y*divisionCount + x;
      if (std::find(managedDivisions[pos].objects.begin(), managedDivisions[pos].objects.end(), a_Object) == managedDivisions[pos].objects.end())
      {
         managedDivisions[pos].objects.push_back(a_Object);
      }
   }
}

void CollisionHandler::addUnmanagedObject(Object* a_Object)
{
   //if(y < 0 || y >= divisionCount || x < 0 || x >= divisionCount))
   {
      if (std::find(unmanagedDivision.objects.begin(), unmanagedDivision.objects.end(), a_Object) == unmanagedDivision.objects.end())
      {
         unmanagedDivision.objects.push_back(a_Object);
      }
   }
}


void CollisionHandler::removeFromDivision(Object* a_Object, int i)
{
   CollisionDivision * curDiv;
   if (i == -1) curDiv = &unmanagedDivision;
   else curDiv = &managedDivisions[i];

   auto j = std::find(curDiv->objects.begin(), curDiv->objects.end(), a_Object);
   while (j != curDiv->objects.end())
   {
      curDiv->objects.erase(j);
      j = std::find(curDiv->objects.begin(), curDiv->objects.end(), a_Object);
   }
}