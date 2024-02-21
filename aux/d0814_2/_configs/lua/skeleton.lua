print "lua:skeleton.lua"

skeletonRadius = characterRadius
skeletonHeight = -100.0

function Serialize_Skeleton(c)
  local x, y, z = c:GetPosition()
  return
  {
    s_id          = c:GetId(),  
    s_type        = c:GetType(),
    s_hp          = c:GetMaxHitPoints(),
    s_speed       = c:GetApproachSpeed(),
    s_orientation = c:GetOrientation(),  
    s_x           = x,
    s_y           = y
  }  
end

function Deserialize_Skeleton(value)
  local c = CreateSkeleton(value.s_type, value.s_hp, value.s_speed, value.s_x, value.s_y)
  c:SetId(value.s_id)  
  c:SetPosition(value.s_x, value.s_y, 0)
  c:ForceOrientation(value.s_orientation)
  return c
end

function Skeleton(c)
   SetTarget(c)
end

function SkeletonKilledEffects(c)
  if c ~= nil then
    local x, y, z = c:GetPosition() 
    Audio_PlayAt("SkeletonKill", x, y, z, 3.0)
    Audio_PlayAt("Hiss1", x, y, z, 1.0)
    c:CompleteEffects()
  end
end

function SkeletonKilled(c)
  local killed = GetPlayer():data().skeletonsKilled
  GetPlayer():data().skeletonsKilled = killed + 1
  local killed = GetPlayer():data().kills
  GetPlayer():data().kills = killed + 1
  SkeletonKilledEffects(c)
end

function MpKill(c, id)
  if c ~= nil then
    local killer = GetCharacterById(id)
    c:SetHitPoints(0.0, killer)
  end
end

function SkeletonHit(c)
  if c ~= nil then
    local xp, yp, zp = c:GetPosition()
    Audio_PlayAt("SkeletonHit", xp, yp, zp, 0.25)
  end
end

function MpSkeletonKilled(c)
  Call_MultiplayerNoLocal(c, "SkeletonKilledEffects")
  Call_MultiplayerNoLocal(c, "MpKill", GetPlayer():GetId())
  SkeletonKilled(c)
end

function MpSkeletonHit(c)
  Call_Multiplayer(c, "SkeletonHit")
end

function CreateSkeleton_Character(c, type, hp, speed, x, y)

   c:SetTypeId(CONSTRAINED_COUNT_ENEMY_TYPE)

   c:AddBehavior(NewAnimationBehavior())
   c:AddBehavior(NewAggressiveApproach())
   c:AddBehavior(NewKillableBehavior())
   c:AddBehavior(NewDieCollapseBehavior())
   c:AddBehavior(NewGeneratedRolePlayer(math.random()))
   c:AddBehavior(NewMeleeAttack())
   
   c:SetLevel(GetLevelNumber()) -- temporary
   c:SetExperience(BASE_EXPERIENCE_MULTIPLIER * GetLevelNumber())

   SetAttributes(c, RandomBetween(hp / 2, hp), GetRandomSpeed(speed))
   c:SetScale(1.0 * characterScale)

   c:SetRadius(skeletonRadius)
   c:SetApproachMinDistance(0)
   c:SetApproachTooFar(600 * math.random() + 100)
   c:AddSignalHandler(SIGNAL_DIED,         "MpSkeletonKilled")   
   c:AddSignalHandler(SIGNAL_DIED,         "DropLoot")
   c:AddSignalHandler(SIGNAL_RECEIVED_HIT, "MpSkeletonHit")
   return c
end

function CreateSkeleton(type, hp, speed, x, y)
   local c = NewCharacter(-1, "Skeleton", type, x, y, 0.0)
   return CreateSkeleton_Character(c, type, hp, speed, x, y)
end

