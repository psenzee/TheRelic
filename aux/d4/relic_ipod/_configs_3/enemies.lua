print "Lua:enemies.lua"

function SetHitAnimation(c)
  local data = c:data()
  local animationName = c:GetAnimationName()
  if animationName ~= 'Walk_ReceiveHit' then
    data.hit_saveAnimation = animationName
    data.hit = 10
    local frame = Animation_GetFrame(c:GetAnimation())
    c:SetAnimation("Walk_ReceiveHit")
    Animation_SetFrame(c:GetAnimation(), frame)
  end
end

function UpdateHitAnimation(c)
  local data = c:data()
  if data.hit == nil then
    return false 
  end
  if data.hit > 0 then
    data.hit = data.hit - 1
    return true
  elseif data.hit == 0 then
    data.hit = nil
    if data.hit_saveAnimation ~= nil then
      c:SetAnimation(data.hit_saveAnimation)
    end
    data.hit_saveAnimation = nil
  end
  return false
end

dofile (MakeReadOnlyPath("skeleton.lua"))
dofile (MakeReadOnlyPath("necromancer.lua"))
dofile (MakeReadOnlyPath("spider.lua"))
dofile (MakeReadOnlyPath("barrel.lua"))

local ENEMIES = { { weight = 0.3, factory = CreateSkeleton,    type = "Skeleton0",    hp =  50, speed = 8.0 },
                  { weight = 0.6, factory = CreateSkeleton,    type = "Skeleton1",    hp =  25, speed = 10.0 },
                  { weight = 0.05, factory = CreateNecromancer, type = "Necromancer0", hp = 100, speed = 11.0 },
                  { weight = 0.05, factory = CreateSpider,      type = "Spider0",      hp = 100, speed = 5.0 },
                  }

function CreateRandomEnemy()

    local type = GetWeighted(ENEMIES, math.random())
    
    local atx = math.random(-128, 128)
    local aty = math.random(-128, 128)
    
    local x, y, z = GetSpawnablePoint(math.random(0, 2000));
    
    (type.factory)(type.type, type.hp, type.speed, atx + x, aty + y, 0.0)

end