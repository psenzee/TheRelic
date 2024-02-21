print "Lua:drifter.lua"

function RandomDrift(c, degree)
  local x, y, z = c:GetPosition()  
  c:SetPositionRaw(x + math.random() * degree - math.random() * degree, y + math.random() * degree - math.random() * degree, z)
end

function GoToPlayer(c)

  if c == nil or not c:IsVisible() then
    return
  end

  local data = c:data()

  local driftSpeed = 5.0
  if data.driftSpeed ~= nil then
    driftSpeed = data.driftSpeed
  end
  local randomDrift = 3.0
  if data.randomDrift ~= nil then
    randomDrift = data.randomDrift
  end    
  local driftRadius = 1000.0
  if data.driftRadius ~= nil then
    driftRadius = data.driftRadius
  end  
  
  local player = GetPlayer()  

  if not IsAlive(player) then
    RandomDrift(c, randomDrift)
    return
  end

  c:SetCollidable(false)
  
  local  x,  y,  z = c:GetPosition()
  local xp, yp, zp = player:GetPosition()
  
  local dx, dy, dz = v3Sub(xp, yp, zp, x, y, z)
  local mag = v3Length(dx, dy, dz)
  
  if mag > driftRadius then
    RandomDrift(c, randomDrift)
    return  
  end
  
  if mag <= playerRadius then
    Heal(GetPlayer(), 20.0)
    BlueGlow(xp, yp)
    c:SetVisible(false)
    return
  end
  
  if mag < 1.0 then
    RandomDrift(c, randomDrift)
    return
  end  
    
  local maxs = 1 / mag * driftSpeed
  
  local xn = x + dx * maxs
  local yn = y + dy * maxs
  local zn = z + dz * maxs
    
  c:SetPosition(xn, yn, zn)  

end

function GoToPlayerAndExecute(c, fn, executeData)
  if c == nil or not c:IsVisible() then
    return
  end

  local data = c:data()

  local driftSpeed = 5.0
  if data.driftSpeed ~= nil then
    driftSpeed = data.driftSpeed
  end
  local randomDrift = 3.0
  if data.randomDrift ~= nil then
    randomDrift = data.randomDrift
  end    
  local driftRadius = 1000.0
  if data.driftRadius ~= nil then
    driftRadius = data.driftRadius
  end  
  
  local player = GetPlayer()  

  if not IsAlive(player) then
    RandomDrift(c, randomDrift)
    return
  end

  c:SetCollidable(false)
  
  local  x,  y,  z = c:GetPosition()
  local xp, yp, zp = player:GetPosition()
  
  local dx, dy, dz = v3Sub(xp, yp, zp, x, y, z)
  local mag = v3Length(dx, dy, dz)
  
  if mag > driftRadius then
    RandomDrift(c, randomDrift)
    return  
  end
  
  if mag <= playerRadius then
    fn(GetPlayer(), executeData)
    BlueGlow(xp, yp)
    c:SetVisible(false)
    return
  end
  
  if mag < 1.0 then
    RandomDrift(c, randomDrift)
    return
  end  
    
  local maxs = 1 / mag * driftSpeed
  
  local xn = x + dx * maxs
  local yn = y + dy * maxs
  local zn = z + dz * maxs
    
  c:SetPosition(xn, yn, zn)  

end