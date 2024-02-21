print "lua:item.lua"

function Serialize_Item(c)
  local x, y, z = c:GetPosition()
  return
  {
    s_id          = c:GetId(),
    s_type        = c:GetType(),
    s_count       = c:data().count,
    s_rotate      = c:GetRotation(),
    s_x           = x,
    s_y           = y
  }  
end

function Deserialize_Item(value)
  local c = CreateItem(value.s_type, value.s_count, value.s_rotate, value.s_x, value.s_y)
  c:SetPosition(value.s_x, value.s_y, 0)
  c:SetId(value.s_id)
end

function AddCountToInventory(c, data)
  AddToInventory(c, data.type, data.count)
end

function Item_AddToInventory(c)
  --AddCountToInventory(GetPlayer(), c:data())
  AddCountToInventory(GetClosestPlayerDefaultPlayer(c), c:data())
  local x, y, z = c:GetPosition()
  Audio_PlayAt("Pickup", x, y, z, 0.125)  
  DestroyCharacter(c) -- self destruct
end

function GetClosestPlayerDefaultPlayer(c)
   local player = GetClosestPlayer(c)
   if player == nil then
     player = GetPlayer()
   end
   return player
end

function Item(c)
   local player = GetClosestPlayer(c)
   if player ~= nil then
     c:SetApproachTarget(player)
   end
end

function CreateItem_Character(c, type, count, rotate, x, y)

   c:AddBehavior(NewDriftApproach())
   c:AddSignalHandler(SIGNAL_ABSORBED, "Item_AddToInventory")

   c:SetMovable(false)
        
   SetAttributes(c, 0, 0)
   c:SetScale(1.0 * characterScale)
   c:SetApproachSpeed(3.0)

   local data = c:data()   
   data.type = type
   data.count = count
   
   c:SetRadius(10.0)
     
   if rotate then
     c:SetRotation(math.random() * 3.141592 * 2.0)
   end
   
   --data.driftRadius = 100.0
   --data.randomDrift = 0.0 
   
   return c
end

function CreateItem(type, count, rotate, x, y)
   local c = NewCharacter(-1, "Item", type, x, y, 0.0)
   return CreateItem_Character(c, type, count, rotate, x, y)
end
