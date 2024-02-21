print "Lua:barrel.lua"

barrelRadius = characterRadius * 0.75
barrelHeight = -100.0

function Serialize_Barrel(c)
  local x, y, z = c:GetPosition()
  return
  {
    s_type        = c:GetType(),
    s_hp          = c:GetMaxHitPoints(),
    s_speed       = c:GetApproachSpeed(),
    s_orientation = c:GetOrientation(),  
    s_x           = x,
    s_y           = y
  }  
end

function Deserialize_Barrel(value)
  local c = CreateBarrel(value.s_type, value.s_hp, value.s_speed, value.s_x, value.s_y)
  c:SetPosition(value.s_x, value.s_y, 0)
  c:ForceOrientation(value.s_orientation)
end

function Barrel(c)
end

function BarrelKilled(c)
  local x, y, z = c:GetPosition() 
  Audio_PlayAt("BarrelKill", x, y, z, 2.0)
  c:CompleteEffects()  
end

function CreateBarrel_Character(c, type, hp, speed, x, y)
   
   c:AddBehavior(NewKillableBehavior())
   c:AddBehavior(NewAnimationBehavior())   
   c:AddBehavior(NewDieCollapseBehavior())
   
   c:SetExperience(0)
   c:SetFilterPosition(true)
      
   SetAttributes(c, math.random(hp / 2, hp), math.random() * speed)
   if math.random() < 0.8 then
     c:SetScale(1.0 * characterScale)
   else
     c:SetScale(1.2 * characterScale) -- special 20% more!
   end
   
   c:SetRadius(barrelRadius)  
   c:SetRotation(math.random() * math.pi)

   c:AddSignalHandler(SIGNAL_DIED, "BarrelKilled")   
   c:AddSignalHandler(SIGNAL_DIED, "DropLootFromBarrel")
   
   return c
end

function CreateBarrel(type, hp, speed, x, y)
   local c = NewCharacter(-1, type, type, x, y, 0.0)
   return CreateBarrel_Character(c, type, hp, speed, x, y)
end