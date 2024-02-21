print "Lua:necromancer.lua"

necromancerRadius = characterRadius
necromancerHeight = -100.0

function NecromancerDie(c, data)
  data.dead = true
  TurnOffHPMeter(c)  
  DropLoot(c)  
  c:SetAnimation("Die")
  local animation = c:GetAnimation()
  Animation_SetFrameTime(animation, 120)
  DestroyPerpetualEffect(data)
--c:SetVisible(false)
  c:SetCollidable(false)
end

function NecromancerReceiveHit(c, hp)
  SetHitAnimation(c)
  local x, y, z = c:GetPosition()  
  local data = c:data()
  FireHit(x, y)
  --DustHit(x, y)
  GenericReceiveHit(c, hp)
end

function Necromancer(c)
  local data = c:data()

  GenericUpdate(c, data)

  if not IsAlive(c) then
    TurnOffHPMeter(c)
    GoToPlayer(c)
    return
  end
 
  local xp, yp, zp = GetPlayer():GetPosition()
  local x,  y,  z  = c:GetPosition()
  local radius = skeletonRadius
    
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
  
  if mag ~= 0 and mag > (skeletonRadius + playerRadius) and data.attackCount == 0 then
  
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
    --else
    --  HandleCollision2(c, radius, 0, 0, 0, xn, yn, zn, true)
    end
    
    --HandleCollision(c, radius, x, y, z, xn, yn, zn, false)
  end
 
  if not isHit then
    SetSpeedAnimation(c)
  end
end

function CreateNecromancer(type, hp, speed, x, y)

   local c = Character.new("Necromancer", type, x, y, 0.0)
   FillAttributeTable(c, math.random(hp / 2, hp), math.random() * speed, { ReceiveHitFn = NecromancerReceiveHit, DieFn = NecromancerDie})
   c:SetScale(1.0 * characterScale)
   local data = c:data()
   data.radius = necromancerRadius
   data.dirx, data.diry, data.dirz = v3Normal(math.random() * 2.0 - 1.0, math.random() * 2.0 - 1.0, 0.0)   
   if math.random() < 0.05 then
     data.cloud = PerpetualFire(x, y, skeletonHeight)
   end
end
