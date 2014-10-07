Vector2 = require 'Vector2' -- include 2d vector lib 

function finishCasting()
    local current = Vector2:new(getOwnerX(), getOwnerY())
    local to = Vector2:new(getSpellToX(), getSpellToY())

    to:sub(current)
    local trueCoords

    if(to:length() > 475) then
        to:normalize()
        local range = Vector2:Mult(to,475)
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

    addParticle("Ezreal_arcaneshift_cas.troy", getOwnerX(), getOwnerY());
    teleportTo(trueCoords.x, trueCoords.y)
    addParticleTarget("Ezreal_arcaneshift_flash.troy", getOwner());
    
    --print("Ezreal E used from Lua, teleporting to " ..trueCoords.x .. " " .. trueCoords.y)
end

function applyEffects()

end