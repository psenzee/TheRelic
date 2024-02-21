print "Lua:loot.lua"

dofile (MakeReadOnlyPath("item.lua"))

local LOOT = { { weight = 0.80, factory = CreateItem, type = "Gold",     rotate = false, count = -50 }, -- a negative count means random between 1 and -count

               { weight = 0.05, factory = CreateItem, type = "Potion0",  rotate = true,  count = 1 },
--             { weight = 0.05, factory = CreateItem, type = "Potion5",  rotate = true,  count = 1 },
--             { weight = 0.05, factory = CreateItem, type = "Potion1",  rotate = true,  count = 1 },
--             { weight = 0.05, factory = CreateItem, type = "Potion3",  rotate = true,  count = 1 },

--             { weight = 0.01, factory = CreateItem, type = "Potion2",  rotate = true,  count = 1 },
--             { weight = 0.01, factory = CreateItem, type = "Potion3",  rotate = true,  count = 1 },
--             { weight = 0.01, factory = CreateItem, type = "Potion7",  rotate = true,  count = 1 },               
--             { weight = 0.05, factory = CreateItem, type = "Potion4",  rotate = true,  count = 1 },
               { weight = 0.01, factory = CreateItem, type = "Potion6",  rotate = true,  count = 1 },
--             { weight = 0.01, factory = CreateItem, type = "Potion8",  rotate = true,  count = 1 },
                                        
               { weight = 0.01, factory = CreateItem, type = "Book2",    rotate = true,  count = 1 },
               { weight = 0.005, factory = CreateItem, type = "Book4",    rotate = true,  count = 1 },
               
--             { weight = 0.015, factory = CreateItem, type = "Potion2",  rotate = true,  count = 1 },
               { weight = 0.05, factory = CreateItem, type = "Potion4",  rotate = true,  count = 1 },
              
               -- ..
               
               }
     
function GetRandomLootType()
  local type = GetWeighted(LOOT, math.random())
  local player = GetPlayer()
  local magic = false
  if player == nil then
    local data = player:data()
    if data ~= nil then
      magic = data.isMagicEnabled
    end
  end
  while
    (type.type == 'Potion0' and GetInventoryItemCount(GetPlayer(), 'Potion0') >= 4) or -- heal potions are a controlled substance
    (type.type == 'Potion4' and not magic)                                          or -- no mana until magic is enabled
    (type.type == 'Potion1' and GetInventoryItemCount(GetPlayer(), 'Potion1') >= 2) or -- portals are also controlled
    (type.type == 'Potion5' and GetInventoryItemCount(GetPlayer(), 'Potion5') >= 2)    -- portals are also controlled
      do 
    type = GetWeighted(LOOT, math.random())
  end  
  
  return type
end

local LOOT_HEAL_INDEX = 2

function GetRealCount(type)
  local count = type.count
  if count < 0 then -- count is negative, means use random
     return 1 + math.random(-count - 1)
  end
  return count
end

function CreateLoot(type, x, y, z)  
  local atx = math.random(-16, 16)
  local aty = math.random(-16, 16);
--(type.factory)(type.type, GetRealCount(type), type.rotate, atx + x, aty + y)
  return CreateAndSerializeItem(type.factory, type.type, GetRealCount(type), type.rotate, atx + x, aty + y)
end

function CreateRandomLoot(x, y, z)
  spawn = LEVELS_SPAWN[LEVEL]
  if (math.random() > spawn.loot_weight) then
    return nil
  end
  local type = GetRandomLootType()
  return CreateLoot(type, x, y, z)
end

function DropLoot(c)
  local x, y, z = c:GetPosition()
  local count = math.random(1, 2)
  for i = 1, count do  
    CreateRandomLoot(x, y, z)
  end
end

function DropLootFromBarrel(c)
  local heal = math.random(1, 2)
  if heal < 1.5 and GetInventoryItemCount(GetPlayer(), 'Potion0') < 4 then
    local x, y, z = c:GetPosition()
    CreateLoot(LOOT[LOOT_HEAL_INDEX], x, y, z)
  else
    DropLoot(c)  
  end
end

function CreateRandomLootRandom()
                  
  local type = GetRandomLootType()
    
  local atx = math.random(-128, 128)
  local aty = math.random(-128, 128);
    
  local x, y, z = GetSpawnablePoint(math.random(0, 2000));    

--(type.factory)(type.type, GetRealCount(type), type.rotate, atx + x, aty + y)
  return CreateAndSerializeItem(type.factory, type.type, GetRealCount(type), type.rotate, atx + x, aty + y)
end

function CreateAndSerializeItem(factory, type, count, rotate, x, y)
  local created = factory(type, count, rotate, x, y, MakeRandomSeed())
  if created == nil then
    breakpoint("Created is nil!")
  end
  if IS_MULTIPLAYER then
    local serializeCreated = SerializeCharacter(created)
    if serializeCreated == nil then
      breakpoint("serializeCreated is nil for type" .. type.type .. "!")    
    end
    Rpc(DeserializeCharacter, serializeCreated)
  end
  return created
end 

function CreateRandomLootAtTile(x, y)

  local type = GetRandomLootType()
    
  -- 256 is the tilesize, should be parameterized    
        
  local atx = math.random(-128, 128) + x * 256
  local aty = math.random(-128, 128) + y * 256
        
--(type.factory)(type.type, GetRealCount(type), type.rotate, atx, aty)
  return CreateAndSerializeItem(type.factory, type.type, GetRealCount(type), type.rotate, atx, aty)
end
