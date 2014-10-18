#include "Map.h"
#include "Game.h"
#include "Unit.h"
#include "CollisionHandler.h"
#include "Minion.h"

Map::Map(Game* game, uint64 firstSpawnTime, uint64 spawnInterval, uint64 firstGoldTime) : game(game), waveNumber(0), firstSpawnTime(firstSpawnTime), firstGoldTime(firstGoldTime), spawnInterval(spawnInterval), gameTime(0), nextSpawnTime(firstSpawnTime), nextSyncTime(0), firstBlood(true), killReduction(true)
{
   collisionHandler = new CollisionHandler(this);
}

Map::~Map()
{ 
   delete collisionHandler; 
}

void Map::update(int64 diff) {
   // collisionHandler->update(diff);
   for(auto kv = objects.begin(); kv != objects.end();) {
      if(kv->second->isToRemove() && kv->second->getAttackerCount() == 0) {
         collisionHandler->removeObject(kv->second);
         delete kv->second;
         kv = objects.erase(kv);
         continue;
      }

      if(kv->second->isMovementUpdated()) {
         game->notifyMovement(kv->second);
         kv->second->clearMovementUpdated();
      }
      
      Unit* u = dynamic_cast<Unit*>(kv->second);

      if(!u) {
         kv->second->update(diff);
         ++kv;
         continue;
      }
      
      for(uint32 i = 0; i < 2; ++i) {
         if(u->getSide() == i) {
            continue;
         }
         
         if(visionUnits[u->getSide()].find(u->getNetId()) != visionUnits[u->getSide()].end() && teamHasVisionOn(i, u)) {
            u->setVisibleByTeam(i, true);
            game->notifySpawn(u);
            visionUnits[u->getSide()].erase(u->getNetId());
            game->notifyUpdatedStats(u, false);
            continue;
         }

         if(!u->isVisibleByTeam(i) && teamHasVisionOn(i, u)) {
            game->notifyEnterVision(u, i);
            u->setVisibleByTeam(i, true);
            game->notifyUpdatedStats(u, false);
         } else if(u->isVisibleByTeam(i) && !teamHasVisionOn(i, u)) {
            game->notifyLeaveVision(u, i);
            u->setVisibleByTeam(i, false);
         }
      }
      
      if(u->buffs.size() != 0){
    
          for(int i = u->buffs.size(); i>0;i--){

              if(u->buffs[i-1]->needsToRemove()){
                  u->buffs.erase(u->getBuffs().begin() + (i-1));
                  //todo move this to Buff.cpp and add every stat
                  u->getStats().addMovementSpeedPercentageModifier(-u->getBuffs()[i-1]->getMovementSpeedPercentModifier());
                  continue;
              }
              u->buffs[i-1]->update(diff);
          }
      }
      
      
      if(!u->getStats().getUpdatedStats().empty()) {
         game->notifyUpdatedStats(u);
         u->getStats().clearUpdatedStats();
      }
      
      if(u->getStats().isUpdatedHealth()) {
         game->notifySetHealth(u);
         u->getStats().clearUpdatedHealth();
      }
      
      if(u->isModelUpdated()) {
         game->notifyModelUpdate(u);
         u->clearModelUpdated();
      }
      
      kv->second->update(diff);
      ++kv;
   }

   for (auto i = announcerEvents.begin(); i != announcerEvents.end(); i++) {
      bool isCompleted = (*i).first;

      if (!isCompleted) {
         uint64 eventTime = std::get<0>((*i).second);
         uint8 messageId = std::get<1>((*i).second);
         bool isMapSpecific = std::get<2>((*i).second);

         if (gameTime >= eventTime) {
            game->notifyAnnounceEvent(messageId, isMapSpecific);
            (*i).first = true;
         }
      }
   }
   
   gameTime += diff;
   nextSyncTime += diff;

   // By default, synchronize the game time every 10 seconds
   if (nextSyncTime >= 10 * 1000000) {
      game->notifyGameTimer();
      nextSyncTime = 0;
   }
   
   if(waveNumber) { 
      if(gameTime >= nextSpawnTime+waveNumber*8*100000) { // Spawn new wave every 0.8s
         if(spawn()) {
            waveNumber = 0;
            nextSpawnTime += spawnInterval;
         } else {
            ++waveNumber;
         }
      }  
   } else if(gameTime >= nextSpawnTime) {
      spawn();
      ++waveNumber;
   }
}

Object* Map::getObjectById(uint32 id) {
   if(objects.find(id) == objects.end()) {
      return 0;
   }
   
   return objects[id];
}

void Map::addObject(Object* o) {
   objects[o->getNetId()] = o;
   
   collisionHandler->addObject(o);

   Unit* u = dynamic_cast<Unit*>(o);
   if(!u) {
      return;
   }
   
   visionUnits[o->getSide()][o->getNetId()] = u;
   
   Minion* m = dynamic_cast<Minion*>(u);
   
   if(m) {
      game->notifyMinionSpawned(m, m->getSide());
   }
   
   Champion* c = dynamic_cast<Champion*>(o);
   
   if(c) {
      game->notifyChampionSpawned(c, c->getSide());
   }
}

void Map::removeObject(Object* o) {
   objects.erase(o->getNetId());
   visionUnits[o->getSide()].erase(o->getNetId());
}

void Map::stopTargeting(Unit* target) {
   for(auto kv = objects.begin(); kv != objects.end(); ++kv) {
      Unit* u = dynamic_cast<Unit*>(kv->second);

      if(!u) {
         continue;
      }
      
      if(u->getUnitTarget() == target) {
         u->setUnitTarget(0);
         u->setLastTarget(0);
         game->notifySetTarget(u, 0);
      }
   }
}

std::vector<Champion*> Map::getChampionsInRange(Target* t, float range) {
	std::vector<Champion*> champs;
	for (auto kv = objects.begin(); kv != objects.end(); ++kv) {
		Champion* u = dynamic_cast<Champion*>(kv->second);

		if (!u) {
			continue;
		}

		if (t->distanceWith(u)<=range) {
			champs.push_back(u);
		}
	}
	return champs;
}

bool Map::teamHasVisionOn(int side, Object* o) {
   

   if(o->getSide() == side) {
      return true;
   }

   for(auto kv : objects) 
   {
      if (kv.second->getSide() == side && kv.second->distanceWith(o) < kv.second->getVisionRadius() && !mesh.isAnythingBetween(kv.second, o))
      {
         Unit * unit = dynamic_cast<Unit*>(kv.second);
         if ((unit) && unit->isDead()) continue;
         return true;
      }
   }
   
   return false;
}

MovementVector Map::toMovementVector(float x, float y) {
   // TODO : do NOT hardcode the width, but for now the AIMesh module reports a bad width (14440)
   return MovementVector((int16)((x-13982/2)/2), (int16)((y-mesh.getHeight()/2)/2));
}