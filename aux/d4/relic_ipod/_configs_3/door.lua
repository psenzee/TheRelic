print "Lua:door.lua"

doorRadius = 50.0
doorHeight = -100.0

function DoorReceiveHit_ById(id)
  DoorChangeState(GetCharacterById(id))
end

function MakeUncollidable_ById(id)
  MakeUncollidable(GetCharacterById(id))
end

function RestoreCollidable_ById(id)
  RestoreCollidable(GetCharacterById(id))
end

function DoorReceiveHit(c, hp)
  DoorChangeState(c)
--Rpc(DoorReceiveHit_ById, c:GetId())
end

function UpdateCharacter_Door(c)
  c:SetFixed(true)
  c:SetCollidable(true)
  c:SetAnimation("Locked")
  local animation = c:GetAnimation()
  Animation_SetFrameTime(animation, 160)
  c:SetMovable(false)
  c:SetScale(1.0)
  c:SetOrientation(c:data().orientation)
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
end

function GetKeyName(c)
  local data = c:data()
  return "Key" .. data.type
end

function DoorChangeState(c)
 
  c:MarkChanged()
  local animName = c:GetAnimationName()
  if animName == "Locked" then
    if not RetrieveFromInventory(GetPlayer(), GetKeyName(c), 1) then     -- sorry can't open it
      c:SetAnimation("DoOpenFail")
      local data = c:data()     
      SendRisingMessage("No " .. data.type .. " Key!", 0)
    else
      c:SetAnimation("DoOpen")
      local x, y, z = c:GetPosition()
      DoorLockHit(x, y)
      MakeUncollidable(c)
      Rpc(MakeUncollidable_ById, c:GetId())
      SendRisingMessage("Unlocked", 0)
    end  
  elseif animName == "Closed" then
    c:SetAnimation("DoOpen")
    MakeUncollidable(c)
    Rpc(MakeUncollidable_ById, c:GetId())
  elseif animName == "Open" then
    c:SetAnimation("DoClose")  
    RestoreCollidable(c)
    Rpc(RestoreCollidable_ById, c:GetId())
  end

end

function CreateDoor_Character(c, type, x, y, direction)
  
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
   
   local c = NewCharacter(-1, "Door", type .. "Door", x, y, 0.0)
   c:data().UpdateCharacterFn = UpdateCharacter_Door   
   
   c:data().orientation = orientation
   c:SetFixed(true)
   c:SetCollidable(true)
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

function CreateDoor(type, x, y, direction)
  -- local c = NewCharacter(-1, "Door", type .. "Door", x, y, 0.0)  
  local c = nil
  CreateDoor_Character(c, type, x, y, direction)
end