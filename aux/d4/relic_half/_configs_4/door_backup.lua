print "lua:door.lua"

doorRadius = 50.0
doorHeight = -100.0

function DoorReceiveHit(c, hp)
  DoorChangeState(c)
end

local toUncollidableMap = { t60  = 60, t61  = 61, t62  = 62, t63  = 63,
                            t64  = 60, t65  = 61, t66  = 62, t67  = 63,
                            t68  = 60, t69  = 61, t70  = 62, t71  = 63,
                            t74  = 60, t75  = 61, t76  = 62, t77  = 63,
                            t78  = 60, t79  = 61, t80  = 62, t81  = 63,
                            t84  = 60, t85  = 61, t86  = 62, t87  = 63,
                            t88  = 60, t89  = 61, t90  = 62, t91  = 63,
                            t104 = 60, t105 = 61, t106 = 62, t107 = 63,
                            t108 = 60, t109 = 61, t110 = 62, t111 = 63 }
                            
function GetUncollidableTile(tileId)
  return toUncollidableMap["t" .. tileId]
end

function MakeUncollidable(c)
  local data = c:data()
  local tile1 = data.tile1
  local tile2 = data.tile2
  SetMapTileIdAt(tile1[1], tile1[2], 0.0, GetUncollidableTile(tile1[3]))
  SetMapTileIdAt(tile2[1], tile2[2], 0.0, GetUncollidableTile(tile2[3]))
end

function RestoreCollidable(c)
  local data = c:data()
  local tile1 = data.tile1
  local tile2 = data.tile2
  SetMapTileIdAt(tile1[1], tile1[2], 0.0, tile1[3])
  SetMapTileIdAt(tile2[1], tile2[2], 0.0, tile2[3])  
end

function Door(c)
  local data = c:data()
  GenericUpdate(c, data)
  if data.stateChange > 0 then
    data.stateChange = data.stateChange - 1
  elseif data.locked then
    c:SetAnimation("Locked")
  elseif data.closed then
    c:SetAnimation("Closed")
  else
    c:SetAnimation("Open")
  end
end

function GetKeyName(c)
  local data = c:data()
  return "Key" .. data.type
end

function DoorChangeState(c)
  local data = c:data()
  if data.stateChange > 0 or (data.locked and not RetrieveFromInventory(GetPlayer(), GetKeyName(c))) then     -- sorry can't open it
    return
  end
  data.stateChange = 100
  if data.closed then
    c:SetAnimation("DoOpen")
    if data.locked then
        local x, y, z = c:GetPosition()
        DoorLockHit(x, y)
        data.locked = false
    end
    local animation = c:GetAnimation()
    Animation_Reset(animation)
    Animation_SetLoopMode(animation, false)
    data.closed = false
    MakeUncollidable(c)
  else
    c:SetAnimation("DoClose")
    local animation = c:GetAnimation()
    Animation_Reset(animation)    
    Animation_SetLoopMode(animation, false)
    data.closed = true
    RestoreCollidable(c)
  end  
end

function CreateDoor(type, x, y, direction)

   local orientation = 0
   local tile1 = { x, y, GetMapTileIdAt(x, y, 0.0) }
   local tile2 = { x, y, 0 }
   
   if direction == 0 then
     orientation = PI
     x = x - 128
     tile2 = { x - 128, y, GetMapTileIdAt(x - 128, y, 0.0) }
   elseif direction == 1 then
     orientation = PI + HALFPI
     y = y + 128
     tile2 = { x, y + 128, GetMapTileIdAt(x, y + 128, 0.0) }
   elseif direction == 2 then
     orientation = 0
     x = x + 128     
     tile2 = { x + 128, y, GetMapTileIdAt(x + 128, y, 0.0) }
   elseif direction == 3 then
     orientation = HALFPI
     y = y - 128
     tile2 = { x, y - 128, GetMapTileIdAt(x, y - 128, 0.0) }
   end

   local c = Character.new("Door", type .. "Door", x, y, 0.0)
   c:SetAnimation("Locked")
   local animation = c:GetAnimation()
   Animation_SetFrameTime(animation, 160)
   c:SetMovable(false)
   FillAttributeTable(c, 0.0, 0.0, { ReceiveHitFn = DoorReceiveHit, DieFn = DoorDie})
   c:SetScale(1.0)
   local data = c:data()
   data.tile1 = tile1
   data.tile2 = tile2
   data.type = type
   data.radius = doorRadius
   c:SetOrientation(orientation)
   data.knockBack = 0.0
   data.closed = true
   data.locked = true
   data.stateChange = 0

end