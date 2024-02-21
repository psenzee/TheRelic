print "Lua:characters.lua"

characterScale = 2.0
characterRadius = characterScale * 20.0

function SetAttributes(c, hpMaximum, maxSpeed)
   c:SetMaxHitPoints(hpMaximum)
   c:SetHitPoints(hpMaximum, c)
   c:SetApproachSpeed(maxSpeed * (characterScale / 2))
   c:SetAttackSpeed(15)   
end

function HPMeter(c, width)
  if c:HasMeter() then
    local data = c:data()
    local xc, yc, zc = c:GetPosition()
    local xp, yp, zp = GetPlayer():GetPosition()
    c:SetMeterDistance(v3Distance(xc, yc, zc, xp, yp, zp))
    c:SetMeterWidth(width)
  end
end

function CharacterDistance(a, b)
  local ax, ay, az = a:GetPosition()
  local bx, by, bz = b:GetPosition()
  return v3Distance(ax, ay, az, bx, by, bz)
end

function CallReceiveHit(c, points)
  if c ~= nil then
    local fn = c:data().methods.ReceiveHitFn
    fn(c, points)    
  end
end

function GenericReceiveHit(c, hp)
  local data = c:data()
  if not c:IsDestroyed() and c:IsKillable() then
    --SetHitAnimation(c)
    if data.invulnerable ~= true then
        --MultiplayerSetAttribute(c, 'hitPoints', data.hitPoints - hp) -- we'll have to do this a different way
        c:SetHitPoints(c:GetHitPoints() - hp, GetPlayer()) -- this GetPlayer() needs to be passed in
        c:MarkChanged()
    end
  end
end

function GenericUpdate(c, data)
  if data.cloud ~= nil then
    local x, y, z = c:GetPosition()
    local height = -100.0
    Effect_SetPosition(data.cloud, x, y, height)
  end
end

function IsAlive(c)
  return not c:IsDestroyed()
end

function GenericDie(c, data)
  c:MarkChanged()
  c:RemoveMeter()
  c:SetAnimation("Die");
  DestroyPerpetualEffect(data)
  local animation = c:GetAnimation()
  Animation_Reset(animation)
  Animation_SetLoopMode(animation, false)
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

function HitCircle_ById(cId, radius, knockBack, points)
  HitCircle(GetCharacterById(cId), radius, knockBack, points)
end

function ReceiveHit_ById(otherId, cId, points, kb)
  local other = GetCharacterById(otherId)
  local c     = GetCharacterById(cId)
  if other ~= nil and c ~= nil then
    other:ReceiveHit(c, points, kb)
  end
end

function MultiplayerReceiveHit(other, c, points, kb)
  other:ReceiveHit(c, points, kb)
  if IS_MULTIPLAYER then
    ReceiveHit_ById(other:GetId(), c:GetId(), points, kb)
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
--       other:KnockBack(c, other:data().knockBack)
         local kb = other:data().knockBack
         if kb == nil then
           kb = 10
         end
         MultiplayerReceiveHit(other, c, points, kb)
--       CallReceiveHit(other, points)
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

function HitCircleInRadius(c, radius, knockBack, points)
   local list = GetAllCharactersInRadius(radius)
   for i = 1, #list do
     local other = list[i]
     local kb = other:data().knockBack
     if kb == nil then
       kb = 10
     end
     MultiplayerReceiveHit(other, c, points, kb)
   end
end

function MultiplayerAreaAttack(c)
  Call_Multiplayer(c, "SetAreaAttackEffect1")
  HitCircle(c, 128.0, 50.0, 10.0)
end