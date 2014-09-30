#ifndef _PATHFINDER_H
#define _PATHFINDER_H

#include <vector>
#include <list>
#include "Vector2.h"
#include "Object.h"

#define MAX_PATHFIND_TRIES 1000

#define TABLE_SIZE (2<<15)
#define GRID_SIZE 256
#define GRID_WIDTH GRID_SIZE
#define GRID_HEIGHT GRID_SIZE

// functions to calculate the G and H factor of the score of the node
#define CALC_H(CURX, CURY, ENDX, ENDY) (abs(CURX - ENDX) + abs(CURY - ENDY))
#define CALC_G(PARENT_G) (PARENT_G+1) // Yes. I know.

#define PATH_DEFAULT_BOX_SIZE 50.0f

class PathNode
{  
public:
   PathNode(){ InitTable(); }
   PathNode(int ax, int ay, int ag, int ah, PathNode *p){ Init(ax, ay, ag, ah, p); }
   PathNode(Vector2 pos, int ag, int ah, PathNode *p){ Init((int)pos.X, (int)pos.Y, ag, ah, p); }
   void Init(int ax, int ay, int ag, int ah, PathNode *p) { InitTable(); x = ax; y = ay; h = ah; g = ag; parent = p; }
   void setScore(int ah, int ag) { g = ag, h = ah; }
   void setParent(PathNode* p) { parent = p; }
   
   void* operator new(size_t size);
   void operator delete(void * objects);
//private:
   int x, y, h, g;
   PathNode* parent;

   static bool isTableEmpty() { InitTable(); return nodeTable.size() == TABLE_SIZE; }
   static unsigned int missingNodes() { InitTable(); return TABLE_SIZE - nodeTable.size(); }
private:
   static void InitTable();
   static std::vector<PathNode*> nodeTable;
   static char tableInitialised;
};

struct Grid
{
   bool isOccupied() { return occupied;/*return (occupant != NULL);*/ }
   bool occupied;
   //Object * occupant;
};

class Map;
class AIMesh;

// TODO: Make Pathfinder threadsafe. Currently it's made to only accept 1 instance, yet we will probably use it for multiple cases
class Pathfinder
{
public:
   Pathfinder():mesh(0),chart(0) {}
   ~Pathfinder() {}

   std::vector<Vector2> getPath(Vector2 from, Vector2 to, float boxSize);

   Vector2 fromGridToPosition(Vector2 position);
   Vector2 fromPositionToGrid(Vector2 position);
   void setMap(Map * map);// { chart = map; mesh = chart->getAIMesh(); }
protected:
   Map * chart;
   AIMesh *mesh;
private:
   void addRealPosToOpenList(Vector2 position, PathNode* parent) 
   { 
      addGridPosToOpenList(fromPositionToGrid(position), parent); 
   }

   void addGridPosToOpenList(Vector2 position, PathNode* parent) 
   { 
      openList.push_back(new PathNode(position, (int)CALC_G((parent)?(parent->g):(0)), (int)CALC_H(position.X, position.Y, destination.X, destination.Y), parent));
   }

   void addToOpenList(Vector2 position, PathNode* parent) 
   { 
      addGridPosToOpenList(position, parent); 
   }

   bool isGridNodeOccupied(Vector2 pos) { isGridNodeOccupied((int)pos.X, (int)pos.Y); }
   bool isGridNodeOccupied(int x, int y);
   PathNode* isNodeOpen(int x, int y);

   void cleanLists();
   void insertObstructions();
private:
   Grid map[GRID_SIZE][GRID_SIZE];
   Vector2 mapTranslation, start, destination;
   float gridNodeSize;

   std::vector<PathNode*> openList, closedList;

   bool traverseOpenList();
   std::vector<Vector2> reconstructPath();
   std::vector<Vector2> reconstructUnfinishedPath();
};

bool SortByF(const PathNode* first, const PathNode* second);
#endif