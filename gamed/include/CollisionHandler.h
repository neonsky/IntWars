#ifndef _COLLISIONHANDLER_H
#define _COLLISIONHANDLER_H

#include "stdafx.h"
#include "Vector2.h"

class Map;
class Object;
class CollisionHandler 
{
public:
	CollisionHandler();
	~CollisionHandler();
	void setMap(Map * a_Map) { m_Map = a_Map; }

	void setCollisionData(Object* a_Pos);
	Object* getCollisionData(Vector2 a_Pos);

	void update(float a_DT);
protected:
	Map * m_Map;
	Object **m_CollisionData;
	uint32 m_Width, m_Height;
};

#endif