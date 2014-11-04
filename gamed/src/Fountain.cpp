#include "Fountain.h"
#include "Map.h"
#include "Logger.h"

void Fountain::setHealLocations(Map* map){
   for (int i = 0; i < NUM_SIDES; i++)
      healLocations.insert(healLocations.begin()+i, new Target(map->getRespawnLoc(i)));
}

void Fountain::healChampions(Map* map, long long diff) {
   healTickTimer += diff;
   if (healTickTimer > 1000000) {
      healTickTimer = 0;

      int side = 0;
      for (auto f : healLocations){
            
         for ( Champion* c : map->getChampionsInRange(f, fountainSize)) {
            if (c->getSide() == side) {
               float HP = c->getStats().getCurrentHealth(), MaxHP = c->getStats().getMaxHealth();
               if (HP + MaxHP * PERCENT_MAX_HEALTH_HEAL < MaxHP) {
                  c->getStats().setCurrentHealth(HP + MaxHP * PERCENT_MAX_HEALTH_HEAL);
               }
               else if (HP < MaxHP) {
                  c->getStats().setCurrentHealth(MaxHP);
                  CORE_INFO("Fully healed at fountain");
               }
            }
         }
         
      side++;
      }

   }
}
