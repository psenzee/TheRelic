print "Lua:barrel.lua"

barrelRadius = characterRadius * 0.75
barrelHeight = -100.0

function BarrelDie(c, data)
  data.dead = true
  c:SetVisible(false)
  TurnOffHPMeter(c)
  DropLoot(c)  
  c:SetAnimation("Die")
  local animation = c:GetAnimation()
  Animation_SetFrameTime(animation, 120)
  DestroyPerpetualEffect(data)
--c:SetVisible(false)
  c:SetCollidable(false)
end

function BarrelReceiveHit(c, hp)
  SetHitAnimation(c)
  local x, y, z = c:GetPosition()  
  local data = c:data()
  --FireHit(x, y)
  DustHit(x, y)
  GenericReceiveHit(c, hp)
end

function Barrel(c)

  local data = c:data()

  GenericUpdate(c, data)

  if not IsAlive(c) then
    TurnOffHPMeter(c)
--  GoToPlayer(c)
    return
  end
 
  local xp, yp, zp = GetPlayer():GetPosition()
  local x,  y,  z  = c:GetPosition()
  local radius = barrelRadius
    
  c:SetOrientation(c:CalculateOrientation())
  
  local dx, dy, dz = v3Sub(xp, yp, zp, x, y, z)
  local mag = v3Length(dx, dy, dz)
  
  HPMeter(c, 1.0)
  
  local isHit = UpdateHitAnimation(c)

  if data.attackCount > 0 then
    data.attackCount = data.attackCount - 1
  elseif not isHit then
    EnemyAttack(c)
  end
  
  if mag ~= 0 and mag > (barrelRadius + barrelRadius) and data.attackCount == 0 then
  
    local maxs = data.maxSpeed
  
    local xn = x + dx / mag * maxs
    local yn = y + dy / mag * maxs
    local zn = z + dz / mag * maxs
    
    if not IsAlive(GetPlayer()) or mag > 500.0 then
      xn = x + data.dirx * maxs
      yn = y + data.diry * maxs
      zn = z + data.dirz * maxs
    end

    local collides = c:CollidesAtRadius(xn, yn, zn, radius)
    if collides == 0 then
      c:SetPosition(xn, yn, zn)
    elseif collides == 512 then
      c:Uncollide()
    end    
  end
 
  if not isHit then
    SetSpeedAnimation(c)
  end
  
end

function CreateBarrel_Character(c, type, hp, speed, x, y)
   --c:SetMovable(false)
   FillAttributeTable(c, math.random(hp / 2, hp), math.random() * speed, { ReceiveHitFn = BarrelReceiveHit, DieFn = BarrelDie})
   if math.random() < 0.8 then
     c:SetScale(1.0 * characterScale)
   else
     c:SetScale(1.2 * characterScale) -- special 20% more!
   end
   local data = c:data()
   data.radius = barrelRadius
   data.knockBack = 10
   c:SetRadius(data.radius)
   c:SetOrientation(math.random() * 6.28)
   data.dirx, data.diry, data.dirz = v3Normal(math.random() * 2.0 - 1.0, math.random() * 2.0 - 1.0, 0.0)   
end

function CreateBarrel(type, hp, speed, x, y)
   local c = NewCharacter(-1, type, type, x, y, 0.0)
   CreateBarrel_Character(c, type, hp, speed, x, y)
end