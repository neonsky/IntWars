#include "Logger.h"
#include "Pathfinder.h"
#include "Map.h"
#include "AIMesh.h"
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include "Logger.h"

Map * Pathfinder::chart = 0;
auto g_Clock = std::clock();

int successes = 0 , oot = 0, empties = 0;

Path Pathfinder::getPath(Vector2 from, Vector2 to, float boxSize)
{
   Path path;
   PathJob job;

   if ((std::clock() - g_Clock) > 10000 && (successes + oot + empties)>0)
   {
		CORE_INFO("Pathfinding successrate: %f", (((float)successes / (float)(successes + oot + empties))*(100.0f)));
   } 

   if (chart == 0) CORE_FATAL("Tried to find a path without setting the map.");
   if (getMesh() == 0) CORE_FATAL("Can't start pathfinding without initialising the AIMesh");

   job.start = job.fromPositionToGrid(from); // Save start in grid info
   job.destination = job.fromPositionToGrid(to); // Save destination in grid info

   job.insertObstructions(chart, getMesh()); // Ready the map.
          
   job.addToOpenList(job.start, 0); // Let's start at the start.

   for (int tries = 0; job.openList.size() != 0; tries++) // Go through the open list while it's not empty
   {
      if (tries == MAX_PATHFIND_TRIES)
      {
         path.error = PATH_ERROR_OUT_OF_TRIES;
         oot++;
         CORE_WARNING("PATH_ERROR_OUT_OF_TRIES");
         path.waypoints = job.reconstructUnfinishedPath();
         job.cleanPath(path);
         job.cleanLists();
         return path;
      }
      else if (job.traverseOpenList(tries==0))
      {
         path.error = PATH_ERROR_NONE;
         successes++;
         CORE_WARNING("We finished a path.");
         path.waypoints = job.reconstructPath();
         job.cleanPath(path);
         job.cleanLists();
         return path;
      }
   }

   CORE_WARNING("PATH_ERROR_OPENLIST_EMPTY");
   path.error = PATH_ERROR_OPENLIST_EMPTY;
   empties++;
   path.waypoints.push_back(from);
   job.cleanPath(path);
   job.cleanLists();
   return path;
}

bool PathJob::traverseOpenList(bool first)
{
   if (openList.size() == 0) return true;
   
   // This sorts every iteration, which means that everything but the last couple of elements are sorted.
   // TODO: That means, this can probably be optimised. Sort only the last elements and add them into the vector where they belong.
   // But honestly, it's running pretty fast so why bother
   std::sort(openList.begin(), openList.end(), SortByF);

   PathNode * currentNode = openList.back();
   openList.pop_back();

   bool atDestination = (currentNode->x == (int)destination.X && currentNode->y == (int)destination.Y);

   if (!atDestination) // While we're not there
   {      
      for (int dx = -1; dx <= 1; dx++)
      {
         if (currentNode->x + dx >= 0 && currentNode->x + dx < GRID_WIDTH) // Search in 8 directions, but we're supposed to stay in map
         {
            for (int dy = -1; dy <= 1; dy++)
            {
               if (!(dx == 0 && dy == 0)) // in y 8 directions, ignore the x==y==0 where we dont move
               {
                  if (!isGridNodeOccupied(currentNode->x + dx, currentNode->y + dy)) // Is something here?
                  {
                     PathNode* conflictingNode = isNodeOpen(currentNode->x + dx, currentNode->y + dy); // Nothing is here, did we already add this to the open list?
                     if (!conflictingNode) // We did not, add it
                     {
                        addToOpenList(Vector2((float)(currentNode->x + dx), (float)(currentNode->y + dy)), currentNode);
                     }
                     else if (conflictingNode->g > CALC_G(currentNode->g)) // I found a shorter route to this node.
                     {
                        conflictingNode->setParent(currentNode); // Give it a new parent
                        conflictingNode->setScore((int)CALC_H(conflictingNode->x, conflictingNode->y, destination.X, destination.Y), CALC_G(currentNode->g)); // set the new score.
                     }
                  }
               }
            }
         }
      }
   }

   closedList.push_back(currentNode);
   return atDestination;
}

std::vector<MovementVector> Path::getWaypoints()
{
	std::vector<MovementVector> ret;
	for (Vector2 waypoint : waypoints)
	{
		ret.push_back(waypoint);
	}

	return ret;
}

std::vector<Vector2> PathJob::reconstructPath( ) // Make a std::vector of the waypoints
{
   std::vector<Vector2> ret;
   auto i = (closedList.back());

   while (i)
   {
      ret.push_back(fromGridToPosition(Vector2((float)i->x, (float)i->y)));
      i = i->parent;
   }

   return ret;
}

std::vector<Vector2> PathJob::reconstructUnfinishedPath() // Let's go over the closed list and go back to the start, create a path from the best choice.
{
   std::vector<Vector2> ret;
   
   auto a = closedList.back();
   int lowestG = 9e7;
   for (auto i = closedList.begin(); i != closedList.end(); i++)
      if ((*i)->g < lowestG)
      {
         lowestG = (*i)->g;
         a = (*i);
      }

   while (a)
   {
      ret.push_back(fromGridToPosition(Vector2((float)a->x, (float)a->y)));
      a = a->parent;
   }

   return ret;
}

void PathJob::cleanPath(Path &path)
{
   if (path.waypoints.size() < 2) return;
   int startSize = path.waypoints.size();
   CORE_WARNING("Cleaning path.. Current size is %d", startSize);

   int dirX = 0, dirY = 0;
   auto prevPoint = path.waypoints.begin();
   for (auto i = path.waypoints.begin()+1; i!=path.waypoints.end(); i++)
   {
      if (((*i).X - (*prevPoint).X == dirX) &&
         ((*i).Y - (*prevPoint).Y == dirY))
      {
         path.waypoints.erase(prevPoint);
         CORE_WARNING("Erased a waypoint");
      }
      else
      {
         dirX = ((*i).X - (*prevPoint).X);
         dirY = ((*i).Y - (*prevPoint).Y);
      }

      prevPoint = i;
   }

   CORE_WARNING("Done cleaning. New size is %d", path.waypoints.size());
   if (startSize != path.waypoints.size())
      CORE_WARNING("Removed %d nodes", startSize-path.waypoints.size());
}

PathNode* PathJob::isNodeOpen(int x, int y)
{
   // TODO: Optimise? This is where the application is residing in 96% of the time during pathfinding.

   // It checks if we've already added this x and y to the openlist. If we did, return it. 
   for (auto i = openList.begin(); i != openList.end(); i++)
   {
      if ((*i)->x == x && (*i)->y == y)
         return (*i);
   }

   return 0;
}

bool SortByF(const PathNode* first, const PathNode* second) // Sort function for finding best score
{
   return (first->g + first->h > second->g + second->h); // Greater than because we want the lower scores in the back (first)
}

Vector2 PathJob::fromGridToPosition(Vector2 position)
{
   AIMesh* mesh = Pathfinder::getMesh();
   if (mesh == 0) CORE_FATAL("Tried to get a grid location without an initialised AIMesh!");
   
   return position*PATH_DEFAULT_BOX_SIZE(mesh->getSize());
}

Vector2 PathJob::fromPositionToGrid(Vector2 position)
{
   AIMesh* mesh = Pathfinder::getMesh();
   if (mesh == 0) CORE_FATAL("Tried to get a position without an initialised AIMesh!");

   return (position / (float)PATH_DEFAULT_BOX_SIZE(mesh->getSize()));
}

bool PathJob::isGridNodeOccupied(int x, int y)
{
   if ((x >= 0 && x < GRID_SIZE) &&
       (y >= 0 && y < GRID_SIZE))
   {
      return map[x][y].isOccupied();
   }
   else return true;
}


void PathJob::insertObstructions(Map * chart, AIMesh *mesh) // insert all objects into the map
{
   std::memset(map, false, sizeof(Grid)*GRID_WIDTH*GRID_HEIGHT); // Empty map

   if (chart == NULL) return;

   const std::map<uint32, Object*>& objects = chart->getObjects();

   for (auto i = objects.begin(); i != objects.end(); i++) // For every object
   {
      Vector2 gridPos = fromPositionToGrid(i->second->getPosition()); // get the position in grid size
      int radius = ((int)(i->second->getCollisionRadius() / gridNodeSize)) / 2; // How many boxes does the radius of this object cover?
      
      for (int dx = -radius; dx < radius; dx++) // For the whole radius in the width
         if (gridPos.X + dx >= 0 && gridPos.X + dx <GRID_WIDTH) // As long as we're in the map (x)
            for (int dy = -radius; dy < radius; dy++) // for the whole radius in the y
               if (gridPos.Y + dy >= 0 && gridPos.Y + dy < GRID_HEIGHT) // As long as we're in the map (y)
                  map[(int)gridPos.X + dx][(int)gridPos.Y + dy].occupied = true; // Occupy this piece of the map.
   }

   if (mesh == NULL) return;

   // Now to draw the mesh onto the thing.
   if (mesh->isLoaded()) // if we have loaded the mesh
      for (int x = 0; x < GRID_WIDTH; x++) // for every grid piece
         for (int y = 0; y < GRID_WIDTH; y++)
         {
            Vector2 translated = fromGridToPosition(Vector2((float)x, (float)y));
            if (mesh->getY(translated.X, translated.Y) < 0.1f) // If there's nothing at this position
               map[x][y].occupied = true; // This is obstructed
         }

}

#ifdef USE_OPTIMISATION
char PathNode::tableInitialised = -1;
std::vector<PathNode*> PathNode::nodeTable;

// Create a table of items to make sure we don't new/delete during runtime
void PathNode::InitTable()
{
   if (tableInitialised!=-1) return; // We have already initialised it or we're busy doing it.
   tableInitialised = 0;
   nodeTable.reserve(TABLE_SIZE);
   for (int i = 0; i < TABLE_SIZE; i++)
   {
      nodeTable.push_back(new PathNode());
   }

   tableInitialised = 1;
}

void* PathNode::operator new(size_t size) // catch news
{
   PathNode *ret;
   switch(tableInitialised)
   {
   case -1: // Should start
      InitTable();

   case 1: // Already cached
      //if (nodeTable.size() == 0) 0;
         //__debugbreak(); // Tried to get a new node while we're all out of nodes.
      ret = nodeTable.back();
      ret->Init(0, 0, 0, 0, 0);
      nodeTable.pop_back();
      break;

   case 0: // creating cache
      ret = (PathNode*)std::malloc(size);
      //if (ret == 0) 0;//__debugbreak(); // No memory?
   }
   return ret;
}

void PathNode::operator delete(void * object)
{
   if (tableInitialised == 2) std::free(object);
   else nodeTable.push_back((PathNode*)object);
}
#endif


void PathJob::cleanLists()
{
   for (auto i = openList.begin(); i != openList.end(); i++) 
      delete (*i);
   openList.clear();
   for (auto i = closedList.begin(); i != closedList.end(); i++) 
      delete (*i);
   closedList.clear();
}

void Pathfinder::setMap(Map * map)
{ 
   chart = map; 
}

AIMesh* Pathfinder::getMesh()
{
   if (!chart) CORE_FATAL("The map hasn't been set but the mesh was requested.");
   return chart->getAIMesh();
}

Path Pathfinder::getPath(Vector2 from, Vector2 to)
{ 
   if (!chart->getAIMesh()) CORE_FATAL("Can't get path because of a missing AIMesh."); 
   return getPath(from, to, PATH_DEFAULT_BOX_SIZE(getMesh()->getSize())); 
}