print "lua:item.lua"

function AddCountToInventory(c, data)
  AddToInventory(c, data.type, data.count)
end

function Item(c)
  local data = c:data()
  GenericUpdate(c, data)
  GoToPlayerAndExecute(c, AddCountToInventory, data)
end

function CreateItem(type, count, rotate, x, y)
   local c = Character.new("Item", type, x, y, 0.0)
   c:SetMovable(false)
   FillAttributeTable(c, 0, 0, { ReceiveHitFn = Nothing2, DieFn = Nothing2})
   c:SetScale(1.0 * characterScale)
   local data = c:data()
   
   data.type  = type
   data.count = count
      
   data.radius = 10.0
   
   if rotate then
     c:SetOrientation(math.random() * 3.141592 * 2.0)
   end
   
   data.driftRadius = 100.0
   data.driftSpeed  = 1.0
   data.randomDrift = 0.0
   data.dead        = true
end