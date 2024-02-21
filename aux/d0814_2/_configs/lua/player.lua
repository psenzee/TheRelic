print "Lua:player.lua"

playerRadius = characterRadius
speedDepth   = 0.0625 -- was 0.5,0.25

function PlayerAttackStart(c)
  local xp, yp, zp = c:GetPosition()
  Audio_PlayAt("PlayerWhoosh", xp, yp, zp, 0.0625)
end

function GetClosest()
  local player = GetPlayer()
  if player ~= nil then
    local closest = GetPlayer():GetClosestFacing(playerRadius * 2.5)
    return closest
  end
  return nil
end

function ResurrectPlayer(c, hpFractionToRestore)
  if IsCharacterValid(c) then
    c:ResetBehaviors()
    c:SetHitPoints(c:GetMaxHitPoints() * hpFractionToRestore, c)
    EnableMovement(c)
    c:SetCollidable(true)
    c:AddSignalHandler(SIGNAL_ATTACK_START, "PlayerAttackStart")
    c:AddSignalHandler(SIGNAL_RECEIVED_HIT, "SkeletonHit")
  end
end

function CreateNewPlayer(id)

   local xs, ys, zs = GetNamedPoint("Start" .. id, 0)
   
   if xs == 0 and ys == 0 then
     xs = x
     ys = y
   end

   local player = NewCharacter(id, "Player", "Player", xs, ys, 0.0)

   player:SetStatic(true)
   player:SetPersistent(true)
   player:SetSelector("blank", 16, 10.0, 35.0, 0.025)
   SetCharacterId(player, id)
   
   player:SetFilterPosition(false)
   player:AddBehavior(NewAnimationBehavior())
   player:AddBehavior(NewKillableBehavior())
   player:AddBehavior(NewDieCollapseBehavior())
   player:AddBehavior(NewRolePlayer())
   player:AddBehavior(NewMeleeAttack())
   player:SetMeterWidth(1.5)
   player:SetRolePlayerDefaultAttributes()
   player:SetAutoAttack(false)
   player:SetKnockSpin(0.0)
   player:SetAttackSpeed(0)
   
   player:SetWeight(2.0)
      
   SetAttributes(player, 2000, 15.0)
   player:SetScale(0.9 * characterScale)
   player:SetMovable(false)
   player:SetVisible(false)
   player:SetRadius(playerRadius)
   
   local data = player:data()
   data.kills = 0
   data.skeletonsKilled = 0
   data.spidersKilled = 0
   data.necromancersKilled = 0
   data.quinotaursKilled = 0   
   data.isMagicEnabled = false
   
   -- $temporary
   data.invulnerable  = false
   data.isLocalPlayer = false
   
   player:AddSignalHandler(SIGNAL_ATTACK_START, "PlayerAttackStart")   
   player:AddSignalHandler(SIGNAL_RECEIVED_HIT, "SkeletonHit")   
   
   data.shield = CreateShield()
   
   return player
end

local lastClosest = nil
local lastFootstep = 0

function Player(c)

  UpdateErrorOccurred("\\#ff8000")

  local data = c:data() 
  
  GenericUpdate(c, data)
  
  if c:IsDestroyed() then
    return
  end
  
  c:SetMeterDistance(10000) -- hide player meter

  data.shield:SetPosition(xp, yp, zp)

  local scale = 8.0
  local xi, yi, zi = 0.0, 0.0, 0.0
  local closest = nil

  if data.isLocalPlayer then
  
    Update_InputDirection(c)

    closest = c:GetClosestFacing(playerRadius * 2.5)
    if closest ~= lastClosest then
      if lastClosest ~= nil then
        lastClosest:RemoveSelector()
      end
      if closest ~= nil then
        local selectEvent = closest:data().selectEvent
        if selectEvent == nil or (selectEvent ~= nil and CharacterDistance(c, closest) < characterRadius * 3.0) then
          closest:SetSelector("ring11", 16, 10.0, 30.0, 0.025)
        else
          closest = nil
        end
      end
      lastClosest = closest
    end
  end
  
  local xp, yp, zp = c:GetPosition()
  local radius = playerRadius
    
  if data.isLocalPlayer then

    Audio_SetListenerPosition(xp, yp, zp)

    if GetInventoryItemCount(c, "Ring") ~= 0 then
      data.isMagicEnabled = true
    end

    if ButtonPressed(0) then 
    --MultiplayerAttack(c)
      if closest ~= nil then
        c:SetAttackTarget(closest)
        local selectEvent = closest:data().selectEvent
        if selectEvent ~= nil and CharacterDistance(c, closest) < characterRadius * 3.0 then
          FireEvent(selectEvent, "")   
        else
          c:TriggerAttack()
        end
      else
        c:TriggerAttack()
      end      
    end
  
    if ButtonPressed(1) then
      InvokeHealPotion()
    end
  
    if ButtonPressed(2) and data.isMagicEnabled then
      MultiplayerAreaAttack(c)
      Audio_PlayAt("Magic", xp, yp, zp, 1.0)
    end
      
    local xv, yv, zv = c:GetVelocity()
    local speedDepthScalar = v3Length(xv, yv, zv)

    if speedDepthScalar < 1 then
      lastFootstep = 1000
    elseif speedDepthScalar > 1 and lastFootstep > 15 then
      lastFootstep = 10
    end 
    
    if lastFootstep == 0 then      
      if speedDepthScalar < 4.0 and speedDepthScalar > 0.8 then
        Audio_PlayAt("Step", xp, yp, zp, 1.0)
      end
      lastFootstep = 10
    end
    lastFootstep = lastFootstep - 1
    
    local speedDepthFilter = 0.05
    if data.speedDepthScalar == nil then
      data.speedDepthScalar = 1.0
    end
    data.speedDepthScalar = (data.speedDepthScalar * (1.0 - speedDepthFilter)) + speedDepthScalar * speedDepthFilter
    if data.speedDepthScalar > 3.0 then
      data.speedDepthScalar = 3.0
    end
    
    SetSpeedDepthScalar(data.speedDepthScalar, speedDepth)
    
  end
  
end
