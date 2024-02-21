print "lua:spider.lua"

spiderRadius = characterRadius * 2.0
spiderHeight = -40.0

function SpiderDie(c, data)
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

function SpiderReceiveHit(c, hp)
  SetHitAnimation(c)
  local x, y, z = c:GetPosition()  
  local data = c:data()
  PoisonHit(x, y)
  GenericReceiveHit(c, hp)
end

function Spider(c)

  c:SetRotation(3.1415926)

  local data = c:data()

  GenericUpdate(c, data)

  if not IsAlive(c) then
    TurnOffHPMeter(c)
    GoToPlayer(c)
    return
  end
 
  local xp, yp, zp = GetPlayer():GetPosition()
  local x,  y,  z  = c:GetPosition()
  local radius = spiderRadius
    
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
  
  if mag ~= 0 and mag > (spiderRadius + playerRadius) and data.attackCount == 0 then
  
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

function SpiderSmall(c)
  c:SetScale(0.5 * characterScale)
  return Spider(c)
end

function CreateSpider(type, hp, speed, x, y)

   local c = Character.new("SpiderSmall", type, x, y, 0.0)
   FillAttributeTable(c, math.random(hp / 2, hp), (speed / 2) + math.random() * speed / 2, { ReceiveHitFn = SpiderReceiveHit, DieFn = SpiderDie})
   c:SetScale(1.0 * characterScale)
   local data = c:data()
   data.radius = spiderRadius
   data.dirx, data.diry, data.dirz = v3Normal(math.random() * 2.0 - 1.0, math.random() * 2.0 - 1.0, 0.0)   

end