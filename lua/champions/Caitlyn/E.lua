Vector2 = require 'Vector2' -- include 2d vector lib 

function finishCasting()
    -- Knockback Start

    
    local current = Vector2:new(getOwnerX(), getOwnerY())
    local to = Vector2:new(0-getSpellToX(), 0-getSpellToY())

    to:add(current)
    local trueCoords

    trueCoords = Vector2:new(current.x, current.y)


    if(to:length() > 400) then
        to:normalize()
        local range = Vector2:Mult(to,400)
        trueCoords = Vector2:new(current.x, current.y)
        trueCoords:add(range)
    elseif(to:length() >= 237.5 and isWalkable(getSpellToX(), getSpellToY()) == false) then
        -- Apply position fix when requested teleport distance is more than half
        to:normalize()
        local range = Vector2:Mult(to,475)
        trueCoords = Vector2:new(current.x, current.y)
        trueCoords:add(range)
    else
        trueCoords = Vector2:new(getSpellToX(), getSpellToY())
    end

    teleportTo(trueCoords.x, trueCoords.y)
    -- Knockback End
    -- Projectile Start
    local current = Vector2:new(getOwnerX(), getOwnerY())
    local to = Vector2:new(getSpellToX(), getSpellToY())
    to:add(current)
    to.normalize()
    local range = Vector2:Mult(to, 1000)
    local trueCoords = Vector2:add(current, range)
    addServerProjectile(trueCoords.x, trueCoords.y)
    -- Projectile End
end

function applyEffects()
    --todo slow and damage enemy
    addParticleTarget("caitlyn_entrapment_cas.troy", getOwner())
end
