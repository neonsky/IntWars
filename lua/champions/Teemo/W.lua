Vector2 = require 'Vector2' -- include 2d vector lib 

function finishCasting()
   local amt = 12 + getSpellLevel()*8
   print("Speed increase" ..amt)
   local buff = Buff.new("MoveQuick", 3.0, BUFFTYPE_TEMPORARY, getOwner())
   buff:setMovementSpeedPercentModifier(amt)
   addBuff(buff, getOwner())
   --addMovementSpeedBuff(getOwner(), amt, 3)
end

function applyEffects()

end