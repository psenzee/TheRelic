print "Lua:player.lua"

playerRadius = characterRadius
speedDepth   = 0.0625 -- was 0.5,0.25

function CreateNewPlayer(id)

   local xs, ys, zs = GetNamedPoint("Start" .. id, 0)
   
   if xs == 0 and ys == 0 then
     xs = x
     ys = y
   end

   local player = NewCharacter(id, "Player", "Player", xs, ys, 0.0)
   
--   player:SetSelector("RunicRing2", 16, 25.0, 40.0, 0.025)
   player:SetSelector("ring12", 16, 10.0, 35.0, 0.025)
   SetCharacterId(player, id)
   FillAttributeTable(player, 800, 15.0, { ReceiveHitFn = GenericReceiveHit, DieFn = GenericDie })
   player:SetScale(0.9 * characterScale)
   player:SetMovable(false)
   player:SetVisible(false)
   
   local data = player:data()
   data.attackPoints = 15
   
   -- $temporary
   data.invulnerable  = false
   data.isLocalPlayer = false
   
   return player
end

function Attack_ById(playerId, enemyId, num)
  Attack(GetCharacterById(playerId), GetCharacterById(enemyId), num)
end

function SetAngleOffset_ById(cId, angle)
  GetCharacterById(cId):SetAngleOffset(angle)
end

function MultiplayerSetAngleOffset(c, angle)
  c:SetAngleOffset(angle)
  Rpc(SetAngleOffset_ById, c:GetId(), angle)
end

function MultiplayerAttack(c)
  local data        = c:data()
  local offsetAngle = -1.2
  local attackNum   = 5
  if data.hit == nil then
    data.hit = 20
  end
  local enemy = c:GetClosestAttackable(playerRadius * 2.0)
  MultiplayerSetAngleOffset(c, offsetAngle)
  Attack(c, enemy, attackNum)
  if enemy ~= nil then
    enemy:MarkChanged()
    Rpc(Attack_ById, c:GetId(), enemy:GetId(), attackNum)
  end
end

local lastClosest = nil

function Player(c)
  local data = c:data()
  
  GenericUpdate(c, data)
  
  if data.dead then
    return
  end
  
  local scale = 800.0  
  local xi, yi, zi = 0.0, 0.0, 0.0

  HPMeter(c, 1.5)

  if data.isLocalPlayer then
  
    xi, yi, zi = GetVectorInput()
    xi, yi, zi = v3Clamp(xi * scale, yi * scale, zi * scale, -10.0, 10.0)

    local closest = c:GetClosestAttackable(playerRadius * 2)
    if closest ~= lastClosest then
      if lastClosest ~= nil then
        lastClosest:RemoveSelector()
      end
      if closest ~= nil then
        --closest:SetSelector("FireSelector", 16, 20.0, 40.0, 0.025)      
        closest:SetSelector("ring11", 16, 10.0, 30.0, 0.025)      
      end
      lastClosest = closest
    end
  end
  
  local xp, yp, zp = c:GetPosition()
  local radius = playerRadius
  
  local isHit = UpdateHitAnimation(c)  
  
  if data.isLocalPlayer then

    if ButtonPressed(0) then 
--    if not isHit and ButtonPressed(0) then -- $TMP
      MultiplayerAttack(c)
    end
  
    if ButtonPressed(1) then
      MultiplayerHeal(c, 200)
    end
  
    if ButtonPressed(2) then
      MultiplayerAreaAttack(c)
    end
    
    if IsButtonDown(0) or v3LengthSq(xi, yi, zi) < 1.0 then
      xi, yi, zi = 0, 0, 0
    end

    local xn, yn, zn = xp + xi, yp + yi, zp + zi
  
    HandleCollision(c, radius, xp, yp, zp, xn, yn, zn, true)

    c:SetOrientation(c:CalculateOrientation())
    if not IsButtonDown(0) and not isHit then
      MultiplayerSetAngleOffset(c, 0.0)
      SetSpeedAnimation(c)
    end
  
    local xv, yv, zv = c:GetVelocity()
    local speedDepthScalar = v3Length(xv, yv, zv)
    local speedDepthFilter = 0.05  
    data.speedDepthScalar = (data.speedDepthScalar * (1.0 - speedDepthFilter)) + speedDepthScalar * speedDepthFilter
    SetSpeedDepthScalar(data.speedDepthScalar, speedDepth)

  --Rpc(UpdateCharacterData, c:GetId(), c:data())
    
  end
  
end
