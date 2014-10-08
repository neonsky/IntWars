Vector2 = require 'Vector2' -- include 2d vector lib 

function finishCasting()
    spellAnimation("Spell3", getOwner())
    addParticleTarget("Graves_Move_OnBuffActivate.troy", getOwner())
end

function applyEffects()

end