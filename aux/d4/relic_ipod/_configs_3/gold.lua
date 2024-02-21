print "lua:gold.lua"

goldRadius = 10.0
goldHeight = -100.0

function GoldDie(c, data)
  data.dead = true
  c:SetCollidable(false)
end

function GoldReceiveHit(c, hp)
end

function Gold(c)

  local data = c:data()
  GenericUpdate(c, data)
  GoToPlayer(c)
  
end

function CreateGold_Character(c, type, hp, speed, x, y)
  
   c:SetMovable(false)
   FillAttributeTable(c, math.random(hp / 2, hp), math.random() * speed, { ReceiveHitFn = Nothing, DieFn = Nothing})
   c:SetScale(1.0 * characterScale)
   local data = c:data()
   data.radius = goldRadius
   if type ~= "Gold" then
     c:SetOrientation(math.random() * 3.141592 * 2.0)
   end
   data.hp = 0
   data.driftRadius = 100.0
   data.driftSpeed  = 1.0
   data.randomDrift = 0.0
   GoldDie(c, data)

end

function CreateGold(id, type, hp, speed, x, y)
   local c = NewCharacter(-1, "Gold", type, x, y, 0.0)
   CreateGold_Character(c, type, hp, speed, x, y)
end