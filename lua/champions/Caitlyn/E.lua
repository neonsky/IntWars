Vector2 = require 'Vector2' -- include 2d vector lib 

function finishCasting()
    -- Apply knockback
    local original = Vector2:new(getOwnerX(), getOwnerY())
    local current = Vector2:new(getOwnerX(), getOwnerY())
    local to = Vector2:new(getSpellToX(), getSpellToY())

    current:sub(to)
    current:normalize()
    
    local range = Vector2:Mult(current,400)
    local trueCoords = Vector2:Add(original, range)

    dashTo(trueCoords.x, trueCoords.y, getOwner())
    setAnimation("RUN", "SPELL3B", getOwner())
    
    -- Create projectile for slowing enemies
    current = Vector2:new(getOwnerX(), getOwnerY())
    to = Vector2:new(getSpellToX(), getSpellToY())
    
    to:sub(current)
    to:normalize()
    
    range = Vector2:Mult(to, 1000)
    trueCoords = Vector2:Add(current, range)
    
    addProjectile(trueCoords.x, trueCoords.y)
    resetAnimations(getOwner())
end

function applyEffects()
    -- Damage target
    dealMagicalDamage(getEffectValue(1)+(0.8*getOwner():getStats():getTotalAp()))
    addParticleTarget("caitlyn_entrapment_cas.troy", getOwner())
    addParticleTarget("caitlyn_entrapment_tar.troy", getTarget())
    
    -- Apply slow
    local duration = getEffectValue(2)
    print("Duration is" .. duration)
    local buff = Buff.new("CaitlynEntrapmentMissile", duration, BUFFTYPE_TEMPORARY, getTarget(), getOwner())
    buff:setMovementSpeedPercentModifier(-getEffectValue(3)) -- negative value to slow target
    addBuff(buff, getTarget())
    addParticleTarget("caitlyn_entrapment_slow.troy", getTarget())
    
    destroyProjectile()
end
