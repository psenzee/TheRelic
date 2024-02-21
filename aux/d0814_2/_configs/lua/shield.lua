print "lua:shield.lua"

function Shield(c)
end

function CreateShield_Character(c)

  -- c:SetFixed(true)
  -- c:SetStatic(true)
   c:AddBehavior(NewAnimationBehavior())

   SetAttributes(c, 10000000.0, 0)
   c:SetScale(characterScale)

   c:SetRadius(characterRadius)
   return c
end

function CreateShield()
   local c = NewCharacter(-1, "Shield", "Shield", 0.0, 0.0, 0.0)
   CreateShield_Character(c)
   return c
end

