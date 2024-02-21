print "lua:spider.lua"

spiderRadius = characterRadius * 1.0
spiderHeight = -40.0

function Serialize_Spider(c)
  local x, y, z = c:GetPosition();
  return
  {
    s_id          = c:GetId(),
    s_type        = c:GetType(),
    s_seed        = c:GetSeed(),
    s_hp          = c:GetMaxHitPoints(),
    s_speed       = c:GetApproachSpeed(),
    s_orientation = c:GetOrientation(),  
    s_x           = x,
    s_y           = y
  }  
end

function Deserialize_Spider(value)
if value.s_seed == nil then value.s_seed = MakeRandomSeed() end
  local c = CreateSpider(value.s_type, value.s_hp, value.s_speed, value.s_x, value.s_y, value.s_seed)
  c:SetId(value.s_id)
  c:SetPosition(value.s_x, value.s_y, 0)
  c:ForceOrientation(value.s_orientation)
  return c  
end


function Serialize_Arachnarex(c)
  local x, y, z = c:GetPosition();
  return
  {
    s_id          = c:GetId(),
    s_type        = c:GetType(),
    s_seed        = c:GetSeed(),
    s_hp          = c:GetMaxHitPoints(),
    s_speed       = c:GetApproachSpeed(),
    s_orientation = c:GetOrientation(),  
    s_x           = x,
    s_y           = y
  }  
end

function Deserialize_Arachnarex(value)
if value.s_seed == nil then value.s_seed = MakeRandomSeed() end
  local c = CreateArachnarex(value.s_type, value.s_hp, value.s_speed, value.s_x, value.s_y, value.s_seed)
--c:SetId(value.s_id)
  c:SetPosition(value.s_x, value.s_y, 0)
  c:ForceOrientation(value.s_orientation)
  return c  
end

function Spider(c)
  c:SetRotation(3.1415926)
  SetTarget(c)
  UpdateImmolationStatus(c)
  LaunchProjectile(c, "Poison", 20.0, 3)
  return c
end

function Arachnarex(c)
  return Spider(c)
end

function SpiderSmall(c)
  c:SetScale(0.5 * characterScale)
  return Spider(c)
end

function MpSpiderKilled(c)
  Call_MultiplayerNoLocal(c, "SpiderKilledEffects")
  Call_MultiplayerNoLocal(c, "MpKill", GetPlayer():GetId())
  SpiderKilled(c)
end

SOUL_COLLECTED_COUNT = 0
SOUL_COLLECTED_MOD   = 1

function CollectedSoul(c)
  if SOUL_COLLECTED_COUNT == 0 then
    ShowSoulTip()
  end
  if SOUL_COLLECTED_COUNT % SOUL_COLLECTED_MOD == 0 then  
    MessageRise("@15\\#ffff40A Soul Collected!", 0)
    SOUL_COLLECTED_MOD = SOUL_COLLECTED_MOD * 2
  else
    MessageRise("A Soul Collected!", 0)
  end
  SOUL_COLLECTED_COUNT = SOUL_COLLECTED_COUNT + 1
  local player = GetPlayer()
  if player ~= nil then
    local hp = player:GetMaxHitPoints()
    player:SetHitPoints(player:GetHitPoints() + hp * 0.05, player)
  end
  DestroyCharacter(c)  
end

function SpiderKilledEffects(c)
  local x, y, z = c:GetPosition()
  Audio_PlayAt("SkeletonKill", x, y, z, 3.0)
  Audio_PlayAt("SpiderKill", x, y, z, 0.5)
  c:CompleteEffects()
end

function SpiderKilled(c)
  local killed = GetPlayer():data().spidersKilled
  GetPlayer():data().spidersKilled = killed + 1
  AddKill(GetPlayer())
  SpiderKilledEffects(c)
end

function CreateSpider_Character(c, type, hp, speed, x, y, seed)

   c:SetTypeId(CONSTRAINED_COUNT_ENEMY_TYPE)
   c:SetSeed(seed)   

   c:AddBehavior(NewAnimationBehavior())
-- c:AddBehavior(NewAggressiveApproach())
   c:AddBehavior(NewHitAndRunApproach())
   c:AddBehavior(NewKillableBehavior())
   c:AddBehavior(NewDieDriftBehavior())
   c:AddBehavior(NewGeneratedRolePlayer(seed))
   c:AddBehavior(NewMeleeAttack())
   
   c:AddSignalHandler(SIGNAL_ABSORBED, "MultiplayerDestroyCharacter")

   c:SetLevel(GetLevelNumber()) -- temporary
   c:SetExperience(BASE_EXPERIENCE_MULTIPLIER * GetLevelNumber())

   SetAttributes(c, RandomBetweenSeeded(seed, hp / 2, hp), GetRandomSpeedSeeded(seed, speed))
 --c:SetScale(1.0 * characterScale)
   c:SetScale(0.5 * characterScale)
   c:SetRotation(3.1415926)   

   c:SetRadius(spiderRadius)
   c:SetApproachMinDistance(0)
   c:SetApproachTooFar(600 * math.random() + 100)   
   c:AddSignalHandler(SIGNAL_DIED, "MpSpiderKilled")
   c:AddSignalHandler(SIGNAL_DIED, "DropLoot")
   c:AddSignalHandler(SIGNAL_RECEIVED_HIT, "MpSkeletonHit")
   c:AddSignalHandler(SIGNAL_DEATH_COMPLETE, "CollectedSoul")
   
   local data = c:data()
   data.projectiles = true

   return c
end

function CreateSpider(type, hp, speed, x, y, seed)
   local c = NewCharacter(-1, "SpiderSmall", type, x, y, 0.0)
   return CreateSpider_Character(c, type, hp, speed, x, y, seed)
end

function GiveWhiteKey()
  AddToPlayerInventoryMax("KeyWhite", 1, 1)
end

function ArachnarexKilled()
  GiveWhiteKey()
  ShowAchievement("Arachnarex")
end

function CreateArachnarex(type, hp, speed, x, y, seed)
   local c = NewCharacter(-1, "Spider", type, x, y, 0.0)
   CreateSpider_Character(c, type, hp, speed * 2.0, x, y, seed)
   c:SetScale(1.0 * characterScale)
   c:AddParticleEffect(PerpetualBlackCloudBigIndependent(x, y), -100)
   c:AddSignalHandler(SIGNAL_DIED, "ArachnarexKilled")   
   return c
end
