print "Lua:necromancer.lua"

necromancerRadius = characterRadius
necromancerHeight = -100.0

function Serialize_Necromancer(c)
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

function Deserialize_Necromancer(value)
  local c = CreateNecromancer(value.s_type, value.s_hp, value.s_speed, value.s_x, value.s_y)
  c:SetId(value.s_id)  
  c:SetPosition(value.s_x, value.s_y, 0)
  c:ForceOrientation(value.s_orientation)
  return c  
end

function Necromancer(c)
   SetTarget(c)
end

function NecromancerKilled(c)
  local killed = GetPlayer():data().necromancersKilled
  GetPlayer():data().necromancersKilled = killed + 1
  local killed = GetPlayer():data().kills
  GetPlayer():data().kills = killed + 1
  NecromancerKilledEffects(c)
end

function MpNecromancerKilled(c)
  Call_MultiplayerNoLocal(c, "NecromancerKilledEffects")
  Call_MultiplayerNoLocal(c, "MpKill", GetPlayer():GetId())
  NecromancerKilled(c)
end

function NecromancerKilledEffects(c)
  local x, y, z = c:GetPosition()  
  Audio_PlayAt("SkeletonKill", x, y, z, 3.0)
  Audio_PlayAt("Hiss2", x, y, z, 2.0)  
  c:CompleteEffects()
end

function CreateNecromancer_Character(c, type, hp, speed, x, y)

   c:SetTypeId(CONSTRAINED_COUNT_ENEMY_TYPE)

   c:AddBehavior(NewAnimationBehavior())
   c:AddBehavior(NewAggressiveApproach())
   c:AddBehavior(NewKillableBehavior())
   c:AddBehavior(NewDieDriftBehavior())
   c:AddBehavior(NewGeneratedRolePlayer(math.random()))   
   c:AddBehavior(NewMeleeAttack())   
   c:AddSignalHandler(SIGNAL_ABSORBED, "MultiplayerDestroyCharacter")
   c:SetAttackSpeed(15)

   c:SetFilterPosition(true)
   
   c:SetLevel(GetLevelNumber() + 5) -- temporary
   c:SetExperience(BASE_EXPERIENCE_MULTIPLIER * GetLevelNumber())   

   SetAttributes(c, RandomBetween(hp / 2, hp), GetRandomSpeed(speed))
   c:SetScale(1.0 * characterScale)

   c:SetRadius(necromancerRadius)
   c:SetApproachMinDistance(0)
   c:SetApproachTooFar(600 * math.random() + 100)   
   c:AddSignalHandler(SIGNAL_DIED, "MpNecromancerKilled")
   c:AddSignalHandler(SIGNAL_DIED, "DropLoot")
   c:AddSignalHandler(SIGNAL_RECEIVED_HIT, "MpSkeletonHit")
   c:AddSignalHandler(SIGNAL_DEATH_COMPLETE, "CollectedSoul")   
   
   if type == "Necromancer1" then
     c:AddParticleEffect(PerpetualFireIndependent(x, y), -128)
     c:SetFilterPosition(false)
   end
   
   return c
end

function CreateNecromancer(type, hp, speed, x, y)
   local c = NewCharacter(-1, "Necromancer", type, x, y, 0.0)
   return CreateNecromancer_Character(c, type, hp, speed, x, y)
end

