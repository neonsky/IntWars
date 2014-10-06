#ifndef _COLLISIONHANDLER_H
#define _COLLISIONHANDLER_H

#include "stdafx.h"
#include "Map.h"
#include "Vector2.h"

class Object;
class CollisionHandler
{
public:
   CollisionHandler(Map*map);// : chart(map) { Pathfinder::setMap(this); }
   ~CollisionHandler() {}

	void update(float a_DT);
private: 
   Map *chart;
};

#endif