Vector2 = require 'Vector2' -- include 2d vector lib 

function finishCasting()
    spellAnimation("Spell3", getOwner())
    addParticleTarget("Graves_Move_OnBuffActivate.troy", getOwner())
    
    local current = Vector2:new(getOwnerX(), getOwnerY())
    local to = Vector2:new(getSpellToX(), getSpellToY())
    
    to:sub(current)
    to:normalize()
    
    local range = Vector2:Mult(to, 450)
    local trueCoords = Vector2:Add(current, range)
    
    dashTo(trueCoords.x, trueCoords.y, getOwner())
end

function applyEffects()

end