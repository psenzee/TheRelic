print "Lua:teleporter.lua"

TeleportTargets = {}

function FindTeleportTargets()
  for j = 1, 64 do
    local prefix    = "Teleport" .. j
    local targetTag = prefix .. "_Target"
    local sourceTag = prefix .. "_Source"
    local count  = GetNamedPointCount(targetTag)
    for i = 0, count - 1 do -- there SHOULD ONLY BE ONE or ZERO!
      local x, y, z = GetNamedPoint(targetTag, i)
      TeleportTargets[sourceTag] = { x, y, z }      
    end
  end
end

function CreateTeleporter(oldPosition, newPosition)
  local self = {}
  self.maxdelay   = 30
  self.delay      = self.maxdelay
  self.old        = oldPosition
  self.new        = newPosition
  self.xyz        = oldPosition
  Darken(-1)
  return self
end

-- returns false when done
function UpdateTeleporter(self)
  if self.delay <= 0 then 
    self.delay = 0
    return false  
  end
  local xyz = self.xyz
  GetPlayer():SetPositionRaw(xyz[1], xyz[2], xyz[3])
  if self.xyz == self.new then
    if HasPlayerTileEvent("OffsetX-") then  
      self.xyz = { xyz[1] - 96.0, xyz[2], xyz[3] }
    end
  end
  if self.delay == self.maxdelay / 2 then
    self.xyz = self.new  
    UnDarken(-1)
  end 
  self.delay = self.delay - 1  
  --print("delay=", self.delay)   
  return true
end

local teleporter = nil
local teleported = false

function CanTeleport()

  if HasPlayerTileEvent("Teleport") then
    local x, y, z = GetPlayerTileCenter()
    local xp, yp, zp = GetPlayer():GetPosition()
    local distance = v3Distance(xp, yp, zp, x, y, z)
    if v3Distance(xp, yp, zp, x, y, z) < 30.0 then -- check if it's in a small radius from the center
      return true
    end
  end
  return false

end

function UpdateTeleportation()

  if teleporter ~= nil then
    if not UpdateTeleporter(teleporter) then
      teleporter = nil
    else
      teleported = true
    end
  elseif CanTeleport() then
  
    if not teleported and teleporter == nil then   
      local count = GetPlayerTileEventCount()
      for i = 1, 64 do
        local tag = "Teleport" .. i .. "_Source"
        if HasPlayerTileEvent(tag) then          
          local xyz = TeleportTargets[tag]
          if xyz ~= nil then            
            local xp, yp, zp = GetPlayer():GetPosition()
            --local xt, yt, zt = GetNamedPoint("Teleport" .. i .. "_Target", 0) -- SHOULD ONLY BE ONE!
            --if v3Length(xp, yp, zp, xt, yt, zt) < 30.0 then -- check if it's in a small radius from the center
            if true then
              local playerPos = { xp, yp, zp }
              teleporter = CreateTeleporter(playerPos, xyz)
              teleported = true
              break
            end
          end    
        end
      end
    end
    
  else
    teleported = false
  end
end
