print "Lua:loot.lua"

dofile (MakeReadOnlyPath("item.lua"))

local LOOT = { { weight = 0.50, factory = CreateItem, type = "Gold",     rotate = false, count = -50 }, -- a negative count means random between 1 and -count

               { weight = 0.10, factory = CreateItem, type = "Potion0",  rotate = true,  count = 1 },
               { weight = 0.10, factory = CreateItem, type = "Potion1",  rotate = true,  count = 1 },
               
               { weight = 0.10, factory = CreateItem, type = "KeyWhite", rotate = true,  count = 1 },
             --{ weight = 0.15, factory = CreateItem, type = "KeyRed",   rotate = true,  count = 1 },
             --{ weight = 0.15, factory = CreateItem, type = "KeyBlue",  rotate = true,  count = 1 },
                    
               { weight = 0.05, factory = CreateItem, type = "Book0",    rotate = true,  count = 1 },
               { weight = 0.05, factory = CreateItem, type = "Book1",    rotate = true,  count = 1 },
                    
               { weight = 0.01, factory = CreateItem, type = "Potion2",  rotate = true,  count = 1 },
               { weight = 0.01, factory = CreateItem, type = "Potion3",  rotate = true,  count = 1 },
               { weight = 0.01, factory = CreateItem, type = "Potion4",  rotate = true,  count = 1 },
               { weight = 0.01, factory = CreateItem, type = "Potion5",  rotate = true,  count = 1 },
               { weight = 0.01, factory = CreateItem, type = "Potion6",  rotate = true,  count = 1 },
               { weight = 0.01, factory = CreateItem, type = "Potion7",  rotate = true,  count = 1 },
               { weight = 0.01, factory = CreateItem, type = "Potion8",  rotate = true,  count = 1 },
                                        
               { weight = 0.01, factory = CreateItem, type = "Book2",    rotate = true,  count = 1 }, 
               { weight = 0.01, factory = CreateItem, type = "Book3",    rotate = true,  count = 1 },
               { weight = 0.01, factory = CreateItem, type = "Book4",    rotate = true,  count = 1 },
              
               -- ..
               
               }
               
function GetRealCount(type)
  local count = type.count
  if count < 0 then -- count is negative, means use random
     return 1 + math.random(-count - 1)
  end
  return count
end

function CreateRandomLoot(x, y, z)

  local type = GetWeighted(LOOT, math.random())
    
  local atx = math.random(-16, 16)
  local aty = math.random(-16, 16);
    
  (type.factory)(type.type, GetRealCount(type), type.rotate, atx + x, aty + y)
end

function DropLoot(c)

  local x, y, z = c:GetPosition()

  local count = math.random(0, 1)
  for i = 1, count do  
    CreateRandomLoot(x, y, z)
  end

end

function CreateRandomLootRandom()
                  
  local type = GetWeighted(LOOT, math.random())
    
  local atx = math.random(-128, 128)
  local aty = math.random(-128, 128);
    
  local x, y, z = GetSpawnablePoint(math.random(0, 2000));    
    
  (type.factory)(type.type, GetRealCount(type), type.rotate, atx + x, aty + y)
end
