print "Lua:characters.lua"

characterScale = 2.0
characterRadius = characterScale * 20.0

function FillAttributeTable(c, hpMaximum, maxSpeed, methods)
   local data            = c:data()
   data.hitPointsMax     = hpMaximum
   data.hitPoints        = hpMaximum
   data.attackPoints     = 5
   data.maxSpeed         = maxSpeed * (characterScale / 2)
   data.killable         = true
   data.dead             = false
   data.experience       = 0
   data.dieFrames        = 30 -- do I really want to do it this way?
   data.methods          = methods
   data.attackCount      = 0
   data.speedDepthScalar = 0
   data.knockBack        = 60.0
end

local _buttons = { false, false, false, false, 
                   false, false, false, false, 
                   false, false, false, false, 
                   false, false, false, false }
                       
function ButtonPressed(index)
  local pressed = IsButtonDown(index)
  if not pressed then
      _buttons[index] = false
  elseif _buttons[index] then
    pressed = false
  else
    pressed = true
    _buttons[index] = true
  end
  return pressed
end

function SetSpeedAnimation(c)

  local data = c:data()
  if data.attackCount > 0 or (data.hit ~= nil and data.hit > 0) then
    --c:SetAnimation("Attack")
  else
    local speed = c:GetSpeed()
    if speed < 0.1 then
      c:SetAnimation("Idle")
    --elseif speed > 2.0 then
    --  c:SetAnimation("Run")
    else
      c:SetAnimation("Walk")
    end
  end

end

function CharacterDistance(a, b)
  local ax, ay, az = a:GetPosition()
  local bx, by, bz = b:GetPosition()
  return v3Distance(ax, ay, az, bx, by, bz)
end

function HPMeter(c, width)

  local FADE_DISTANCE = 256.0

  local data = c:data()
  local xc, yc, zc = c:GetPosition()
  local xp, yp, zp = GetPlayer():GetPosition()
  local sizex, sizey = -30.0, -7.5
  local hp = data.hitPoints / data.hitPointsMax  
  local id = c:__tostring()
  local d = v3Distance(xc, yc, zc, xp, yp, zp)
  if d > FADE_DISTANCE then
    d = FADE_DISTANCE
  end
  local alpha = 1.0 - d / FADE_DISTANCE
  local idhpbar = "" .. id .. "HPBar"
  local idhp    = "" .. id .. "HP"
  local height = -150.0
  --xc = xc + 20
  yc = yc + 10
  if alpha < 0.1 or hp > 0.95 then
    RemoveQuad("" .. id .. "HPBar")
    RemoveQuad("" .. id .. "HP")
  else
    SetQuad("" .. id .. "HPBar", "health_bar_7",         alpha, xc,     yc,     height, xc - (sizex * width * alpha),     yc - (sizey * alpha),     height, 1,  0, 0,  1)
    SetQuad("" .. id .. "HP",    "health_small_shadow2", alpha, xc + 1, yc + 1, height, xc - (sizex * width * alpha) - 1, yc - (sizey * alpha) - 1, height, 1, hp, 0, hp)
  end
end

function TurnOffHPMeter(c)
  local id = c:__tostring()
  RemoveQuad("" .. id .. "HPBar")
  RemoveQuad("" .. id .. "HP")
end

function Heal(c, hp)
  local data = c:data()
  data.hitPoints = data.hitPoints + hp
  if data.hitPoints > data.hitPointsMax then
    data.hitPoints = data.hitPointsMax
  end
end

function CallReceiveHit(c, points)
  if c ~= nil then
    local fn = c:data().methods.ReceiveHitFn
    fn(c, points)
  end
end

function KnockBack(a, b)
  local xa, ya, za = a:GetPosition()
  local xb, yb, zb = b:GetPosition()
  local dx, dy, dz = v3Normal(xa - xb, ya - yb, za - zb)
  local data = a:data()
  local back = data.knockBack
  a:SetPositionRaw(xa + back * dx, ya + back * dy, za + back * dz)
end

function Attack(c, enemy, hp)
  c:SetAnimation("Attack")
  local animation = c:GetAnimation()
  Animation_Reset(animation)
  Animation_SetLoopMode(animation, false)  
--Animation_SetFrameTime(animation, 20)
  Animation_SetFrameTime(animation, 35)  
  if enemy ~= nil then
    if enemy ~= GetPlayer() then -- don't knock back the player, too jittery looking
      KnockBack(enemy, c)
    end
    CallReceiveHit(enemy, c:data().attackPoints)
  end
end

function GenericReceiveHit(c, hp)
  local data = c:data()
  if not data.dead and data.killable then
    --SetHitAnimation(c)
    if data.invulnerable ~= true then    
        data.hitPoints = data.hitPoints - hp
    end
    -- knock back here
    if data.hitPoints <= 0 then
      data.hitPoints = 0
      local die = data.methods.DieFn
      die(c, data)
      if c ~= GetPlayer() then
        local pdata = GetPlayer():data()
        local exp = 10
        pdata.experience = pdata.experience + exp
        print("Exp : ", pdata.experience)
--      SendRisingMessage("+" .. exp .. "xp", 0)
      end
    end
  end
end

function GenericUpdate(c, data)
  if data.cloud ~= nil then
    local x, y, z = c:GetPosition()
    local height = -100.0
    Effect_SetPosition(data.cloud, x, y, height)
  end
  if data.dead and data.dieFrames > 0 then
    data.dieFrames = data.dieFrames - 1
    if data.dieFrames == 0 then
    --c:SetVisible(false)
      c:SetCollidable(false)
    end
  end
end

function IsAlive(c)
  local data = c:data()
  return not data.dead
end

function GenericDie(c, data)
  data.dead = true
  TurnOffHPMeter(c)
  c:SetAnimation("Die");
  DestroyPerpetualEffect(data)
  local animation = c:GetAnimation()
  Animation_Reset(animation)
  Animation_SetLoopMode(animation, false)
  --Animation_SetFrameTime(animation, 67)
end

function EnemyAttack(c)
  local data = c:data()
  local xc, yc, zc = c:GetPosition()
  local xp, yp, zp = GetPlayer():GetPosition()
  local d = v3Distance(xc, yc, zc, xp, yp, zp)
  if d <= (data.radius + playerRadius) * 1.25 and math.random() > 0.8 and c:IsFacing(xp, yp, zp, 1.0) then
    Attack(c, GetPlayer(), 5)
    data.attackCount = 30
  end
end

function HandleCollision(c, radius, xp, yp, zp, xn, yn, zn, uncollideAll)
  local collides = c:CollidesAtRadius(xn, yn, zn, radius * 0.5)
  if collides == 0 then
    c:SetPosition(xn, yn, zn)
  elseif collides == 512 then
    c:SetPosition(xn, yn, zn)
    if uncollideAll then
      c:UncollideAll()
    end
  else
    if not uncollideAll then
      c:Uncollide()
    end
  end
end

function HitCircle(c, radius, knockBack, points)

   local closest = playerRadius + skeletonRadius
-- $TODO this needs to be fixed with an iterator over closest
   local list = {}
   local collidables = {}
   local other = c:GetClosest(closest)
   while other ~= nil do
     table.insert(collidables, other:IsCollidable())
     table.insert(list, other)     
     other:SetCollidable(false)
     local distance = CharacterDistance(c, other)
     if distance < radius then
--     KnockBack(other, c, (distance / radius) * knockBack)
       KnockBack(other, c)
       CallReceiveHit(other, points)
     end
     other = c:GetClosest(closest)
   end
   for i = 1, #list do
     local character  = list[i]
     local collidable = collidables[i]
     if IsAlive(character) then
       character:SetCollidable(collidable)
     end
   end
   
end