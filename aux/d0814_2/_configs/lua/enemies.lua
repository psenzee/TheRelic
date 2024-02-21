print "Lua:enemies.lua"

function TurnEnemies(seconds)
  LOCAL_GAME_STATE.TurnEnemies = true
  if GetTimerName ~= "turn" then
    SetTimerName("turn")
    SetTimerDuration(seconds)
  else
    AddTimerDuration(seconds)
  end
end

function TurnExpiredListener(info, data)
  print("***** TURN EXPIRED CALLED! *****")
  if LOCAL_GAME_STATE.TurnEnemies then
    LOCAL_GAME_STATE.TurnEnemies = false
  end
end

function StartEnemies()
  RegisterListener("HudTimerExpired", "TurnExpiredListener", {})
end

function SetTarget(c)
  if LOCAL_GAME_STATE.TurnEnemies then
    SetTargetAsClosest(c)  
  else
    c:SetApproachTarget(GetPlayer())
    c:SetAttackTarget(GetPlayer())
  end
end

function SetTargetAsClosest(c)
  local toAttack = c:GetClosestAttackable(320.0)
  if toAttack == nil then
    toAttack = GetPlayer()
  end
  c:SetApproachTarget(toAttack)
  c:SetAttackTarget(toAttack)
end

dofile (MakeReadOnlyPath("skeleton.lua"))
dofile (MakeReadOnlyPath("necromancer.lua"))
dofile (MakeReadOnlyPath("spider.lua"))
dofile (MakeReadOnlyPath("quinotaur.lua"))
dofile (MakeReadOnlyPath("generatorghost.lua"))
dofile (MakeReadOnlyPath("barrel.lua"))
dofile (MakeReadOnlyPath("guide.lua"))

--[[
local ENEMIES = { { weight = 0.125,  factory = CreateSkeleton,    type = "Skeleton0",    hp =  50, speed = 8.0  },
                  { weight = 0.125,  factory = CreateSkeleton,    type = "Skeleton1",    hp =  25, speed = 10.0 },
                  { weight = 0.125,  factory = CreateSkeleton,    type = "Skeleton2",    hp =  25, speed = 10.0 },
                  { weight = 0.125,  factory = CreateSkeleton,    type = "Skeleton3",    hp =  25, speed = 10.0 },
                  { weight = 0.01,   factory = CreateSkeleton,    type = "Skeleton4",    hp =  25, speed = 10.0 },
                  { weight = 0.01,   factory = CreateSkeleton,    type = "Skeleton5",    hp =  25, speed = 10.0 },
                  { weight = 0.01,   factory = CreateSkeleton,    type = "Skeleton6",    hp =  25, speed = 10.0 },
                  { weight = 0.01,   factory = CreateSkeleton,    type = "Skeleton7",    hp =  25, speed = 10.0 },
                  { weight = 0.05,   factory = CreateNecromancer, type = "Necromancer0", hp = 100, speed = 11.0 },
                  { weight = 0.025,  factory = CreateSpider,      type = "Spider0",      hp = 200, speed = 5.0  },
                  { weight = 0.025,  factory = CreateSpider,      type = "Spider1",      hp = 200, speed = 5.0  },
                  { weight = 0.025,  factory = CreateSpider,      type = "Spider2",      hp = 300, speed = 5.0  },
                  { weight = 0.0125, factory = CreateSpider,      type = "Spider3",      hp = 400, speed = 5.0  },
                  { weight = 0.0125, factory = CreateSpider,      type = "Spider4",      hp = 600, speed = 5.0  },
                  }
]]--


local ENEMIES = { { weight = 0.2,  factory = CreateSkeleton,    type = "Skeleton0",    hp = 250,  speed = 4.0 },
                  { weight = 0.6,  factory = CreateSkeleton,    type = "Skeleton1",    hp = 100,  speed = 5.0 },
                  { weight = 0.05, factory = CreateNecromancer, type = "Necromancer0", hp = 1000, speed = 6.0 },
                --{ weight = 0.05, factory = CreateNecromancer, type = "Necromancer1", hp = 1500, speed = 12.0 },
                  { weight = 0.05, factory = CreateSpider,      type = "Spider0",      hp = 500,  speed = 2.0 },
                  { weight = 0.05, factory = CreateSpider,      type = "Spider1",      hp = 500,  speed = 2.0 },
                  }
                  
local ENEMIES_DIFFICULT
              = {--{ weight = 0.25, factory = CreateSkeleton,    type = "Skeleton6",    hp = 1000, speed = 4.0 },
                --{ weight = 0.25, factory = CreateSkeleton,    type = "Skeleton7",    hp = 1000, speed = 5.0 },
                  { weight = 0.50, factory = CreateNecromancer, type = "Necromancer1", hp = 2000, speed = 6.0 },
                  { weight = 0.50, factory = CreateSpider,      type = "Spider4",      hp = 1000, speed = 2.0 },
                  }                  
                  
function CreateAndSerialize(factory, type, hp, speed, x, y, z)
  local created = factory(type, hp, speed, x, y, z)
  if created == nil then
    breakpoint("Created is nil!")
  end
  if IS_MULTIPLAYER then
    local serializeCreated = SerializeCharacter(created)
    if serializeCreated == nil then
      breakpoint("serializeCreated is nil for type" .. type.type .. "!")    
    end
    Rpc(DeserializeCharacter, serializeCreated)
  end
  return created
end                  

function CreateRandomEnemy()
    local type = GetWeighted(ENEMIES, math.random())
    local atx = math.random(-128, 128)
    local aty = math.random(-128, 128)
    local x, y, z = GetSpawnablePoint(math.random(0, 2000));   
    return CreateAndSerialize(type.factory, type.type, type.hp * (1.0 + GetLevelNumber() * 0.15), type.speed * (1.0 + GetLevelNumber() * 0.10), atx + x, aty + y, 0.0)
end

function CreateRandomEnemyAtTile(x, y, difficult)
    local from = ENEMIES
    if difficult then from = ENEMIES_DIFFICULT end
    local type = GetWeighted(from, math.random())
    
    -- 256 is the tilesize, should be parameterized    
        
    local atx = math.random(-128, 128) + x * 256
    local aty = math.random(-128, 128) + y * 256
    
    if GetLevelNumber() > 1 then
      if     type.type == "Skeleton0" then
        type.type = "Skeleton" .. tostring((GetLevelNumber() - 1) % 8)
      elseif type.type == "Skeleton1" then
        type.type = "Skeleton" .. tostring(GetLevelNumber() % 8)
      elseif type.type == "Spider0" then
        type.type = "Spider" .. tostring((GetLevelNumber() - 1) % 5)
      elseif type.type == "Spider1" then
        type.type = "Spider" .. tostring(GetLevelNumber() % 5)
      end
    end

    return CreateAndSerialize(type.factory, type.type, type.hp * (1.0 + GetLevelNumber() * 0.15), type.speed * (1.0 + GetLevelNumber() * 0.10), atx + x, aty + y, 0.0)
end
