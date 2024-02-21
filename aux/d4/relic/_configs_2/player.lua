print "Lua:player.lua"

playerRadius = characterRadius
speedDepth   = 0.0625 -- was 0.5,0.25

function CreateNewPlayer(x, y)

   local xs, ys, zs = GetStartPoint();
   
   if xs == 0 and ys == 0 then
     xs = x
     ys = y
   end

   local player = Character.new("Player", "Player", xs, ys, 0.0)
   FillAttributeTable(player, 1300, 15.0, { ReceiveHitFn = GenericReceiveHit, DieFn = GenericDie })
   SetPlayer(player)
   player:SetScale(0.9 * characterScale)
   player:SetMovable(false)
   
   local data = player:data()
   data.attackPoints = 15
   
   -- $temporary
   data.invulnerable = false
   
end

function Player(c)
  local data = c:data()
  
  GenericUpdate(c, data)
  
  if data.dead then
    return  
  end
  
  local scale = 800.0  
  local xi, yi, zi = GetVectorInput()  
  xi, yi, zi = v3Clamp(xi * scale, yi * scale, zi * scale, -10.0, 10.0)
  local xp, yp, zp = c:GetPosition()
  local radius = playerRadius

  HPMeter(c, 1.5)
  
  local isHit = UpdateHitAnimation(c)  
  
  if not isHit and ButtonPressed(0) then
    if data.hit == nil then
       data.hit = 20
    end
    local enemy = c:GetClosestAttackable(radius * 2.0)
    c:SetAngleOffset(-1.2)
    Attack(c, enemy, 5)
  end
  
  if ButtonPressed(1) then
    Heal(c, 200)
  end
  
  if ButtonPressed(2) then
    HitCircle(c, 128.0, 50.0, 10.0)
    DustCircle(xp, yp)
  end
  
  if IsButtonDown(0) or v3LengthSq(xi, yi, zi) < 1.0 then
    xi, yi, zi = 0, 0, 0
  end

  local xn, yn, zn = xp + xi, yp + yi, zp + zi
  
  HandleCollision(c, radius, xp, yp, zp, xn, yn, zn, true)

  c:SetOrientation(c:CalculateOrientation())
  if not IsButtonDown(0) and not isHit then
    c:SetAngleOffset(0.0)
    SetSpeedAnimation(c)
  end
  
  local xv, yv, zv = c:GetVelocity()
  local speedDepthScalar = v3Length(xv, yv, zv)
  local speedDepthFilter = 0.05
  data.speedDepthScalar = (data.speedDepthScalar * (1.0 - speedDepthFilter)) + speedDepthScalar * speedDepthFilter
  SetSpeedDepthScalar(data.speedDepthScalar, speedDepth)
end
