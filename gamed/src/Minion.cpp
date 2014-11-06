#include "Minion.h"
#include "MinionStats.h"
#include "Map.h"
#include "Game.h"
#include "Pathfinder.h"
#include "Logger.h"

using namespace std;

Minion::Minion(Map* map, uint32 id, MinionSpawnType type, MinionSpawnPosition position, const vector<Vector2>& mainWaypoints) : Unit(map, id, "", new MinionStats(), 40, 0, 0, 1100), type(type), spawnPosition(position), mainWaypoints(mainWaypoints), curMainWaypoint(0)
{
	// Setup spawn position and team.
	auto spawnSpecifics = map->getMinionSpawnPosition(spawnPosition);
	setTeam(spawnSpecifics.first);
	setPosition(spawnSpecifics.second.X, spawnSpecifics.second.Y);

	map->setMinionStats(this); // Let the map decide how strong this minion has to be.
   
   std::string minionModel;
	if (spawnSpecifics.first==0) // If we're the blue side
		minionModel += "Blue_Minion_"; // make it a blue minion
	else minionModel += "Red_Minion_"; // otherwise make it a red minion
   
	// Finish model name with type
   if (type == MINION_TYPE_MELEE) minionModel += "Basic";
   else if (type == MINION_TYPE_CASTER)  minionModel += "Wizard"; 
   else minionModel += "MechCannon";

	// Set model
   setModel(minionModel);
   
   vector<Vector2> newWaypoints;
   
   if(mainWaypoints.size() > 0)
      newWaypoints = { mainWaypoints[0], mainWaypoints[0] };
	else newWaypoints = { Vector2(x, y), Vector2(x, y) };
   
   setWaypoints(newWaypoints);
   setMoveOrder(MOVE_ORDER_ATTACKMOVE);
}

void Minion::update(int64 diff) 
{
	Unit::update(diff);

	if (!isDead())
	{
		if (scanForTargets())     // returns true if a target is in range
			keepFocussingTarget(); // fight target
		else walkToDestination(); // walk to destination (or target)
	}
}

bool Minion::scanForTargets()
{
	if (!unitTarget) // if we have got no unitTarget
	{
		focusTargetInRange();
	}

	if (unitTarget) 
	{
		Champion* c = dynamic_cast<Champion*>(unitTarget);

		if (c && !autoAttackFlag) 
		{
			const std::map<uint32, Object*>& objects = map->getObjects();
			Unit* nextTarget = 0;
			unsigned int nextTargetPriority = 10;
			for (auto& it : objects) 
			{
				Unit* u = dynamic_cast<Unit*>(it.second);

				if (!u || u->isDead() || u->getTeam() == getTeam() || distanceWith(u) > stats->getRange()) continue;

				// Find the next champion in range targeting an enemy champion who is also in range
				Champion* nextChampion = dynamic_cast<Champion*>(u);
				if (nextChampion && c != nextChampion && nextChampion->getUnitTarget() != 0) 
				{
					Champion* target = dynamic_cast<Champion*>(nextChampion->getUnitTarget());
					if (target && nextChampion->distanceWith(target) <= nextChampion->getStats().getRange() && distanceWith(target) <= stats->getRange()) 
					{
						nextTarget = nextChampion; // No priority required
						break;
					}
				}

				auto priority = classifyTarget(u);
				if (priority < nextTargetPriority) 
				{
					nextTarget = u;
					nextTargetPriority = priority;
				}
			}
			if (nextTarget) 
			{
				setUnitTarget(nextTarget); // Set the new target and refresh waypoints
				map->getGame()->notifySetTarget(this, nextTarget);
			}
		}

		return true;
	}

	return false;
}

void Minion::focusTargetInRange()
{
	Unit* nextTarget = 0;
	unsigned int nextTargetPriority = 10;

	const std::map<uint32, Object*>& objects = map->getObjects();
	for (auto& it : objects)
	{
		Unit* u = dynamic_cast<Unit*> (it.second);

		if (!u || u->isDead() || u->getTeam() == getTeam() || distanceWith(u) > DETECT_RANGE || !getMap()->teamHasVisionOn(getTeam(), u))
			continue;

		auto priority = classifyTarget(u);
		if (priority < nextTargetPriority)
		{
			nextTarget = u;
			nextTargetPriority = priority;
		}
	}

	if (nextTarget)
	{
		setUnitTarget(nextTarget); // Set the new target and refresh waypoints
		map->getGame()->notifySetTarget(this, nextTarget);
	}
}

void Minion::keepFocussingTarget()
{
	if (autoAttackFlag && (!unitTarget || distanceWith(unitTarget) > stats->getRange()))
	{
		map->getGame()->notifyStopAutoAttack(this);
		autoAttackFlag = false;
	}
}

void Minion::walkToDestination()
{ 
   if((waypoints.size() == 1) || (curWaypoint == 2 && ++curMainWaypoint < mainWaypoints.size())) 
	{
      //CORE_INFO("Minion reached ! Going to %d;%d", mainWaypoints[curMainWaypoint].x, mainWaypoints[curMainWaypoint].y);
      vector<Vector2> newWaypoints = { Vector2(x, y), mainWaypoints[curMainWaypoint] };
      setWaypoints(newWaypoints);
   }
}

void Minion::onCollision(Object * a_Collider)
{
}