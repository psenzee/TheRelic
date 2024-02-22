print "lua:item.lua"

function Serialize_Item(c)
  local x, y, z = c:GetPosition()
  return
  {
    s_id          = c:GetId(),
    s_type        = c:GetType(),
    s_seed        = c:GetSeed(),
    s_count       = c:data().count,
    s_rotate      = c:GetRotation(),
    s_x           = x,
    s_y           = y
  }  
end

function Deserialize_Item(value)
  if value.s_seed == nil then value.s_seed = MakeRandomSeed() end
  local c = CreateItem(value.s_type, value.s_count, value.s_rotate, value.s_x, value.s_y, value.s_seed)
  c:SetPosition(value.s_x, value.s_y, 0)
--c:SetId(value.s_id)
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

function ItemSelector(c)
  c:SetSelector("yellow2", 16, 20.0, 25.0, 0.025)
end

function Item(c)
   local player = GetClosestPlayer(c)
   if player ~= nil then
     c:SetApproachTarget(player)
   end
end

function CreateItem_Character(c, type, count, rotate, x, y, seed)

  c:SetSeed(seed)

   c:AddBehavior(NewDriftApproach())   
   c:AddSignalHandler(SIGNAL_ABSORBED, "Item_AddToInventory")

   c:SetMovable(false)
        
   SetAttributes(c, 0, 0)
   c:SetScale(1.0 * characterScale)
   c:SetApproachSpeed(math.random() * 3.0 + 1.0)

   if type == "KeyRed" or 
      type == "KeyBlue" or 
      type == "KeyWhite" or 
      type == "Ring" or 
      type == "Potion2" or
      type == "Potion7" or
      type == "Book2" or
      type == "Book4" then
     c:SetApproachSpeed(0)
   end

   local data = c:data()   
   data.type = type
   data.count = count
   data.selector = ItemSelector
   
   c:SetRadius(10.0)
     
   if rotate then
     c:SetRotation(math.random() * 3.141592 * 2.0)
   end
   
   --data.driftRadius = 100.0
   --data.randomDrift = 0.0 
   
   return c
end

function CreateItem(type, count, rotate, x, y, seed)
   local c = NewCharacter(-1, "Item", type, x, y, 0.0)
   return CreateItem_Character(c, type, count, rotate, x, y, seed)
end
