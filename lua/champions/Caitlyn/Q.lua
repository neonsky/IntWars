Vector2 = require 'Vector2' -- include 2d vector lib 

function finishCasting()
    local current = Vector2:new(getOwnerX(), getOwnerY())
    local to = Vector2:new(getSpellToX(), getSpellToY())

    to:sub(current)
    to:normalize()

    local range = Vector2:Mult(to,1150)
    local trueCoords = Vector2:Add(current, range)

    addServerProjectile(trueCoords.x, trueCoords.y)
end

function applyEffects()

   local reduc = math.min(getNumberObjectsHit(), 5)

   dealPhysicalDamage((getEffectValue(0)+getOwner():getStats():getTotalAd()+(1.3*getOwner():getStats():getTotalAd()))+getEffectValue(0)*(1-reduc/10.0))
   
   addParticleTarget("caitlyn_Base_peaceMaker_tar_02.troy", getTarget())
end