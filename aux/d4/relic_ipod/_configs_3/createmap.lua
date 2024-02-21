print "Lua:createmap.lua"

ENEMIES_MAX = 300
LOOT_MAX    = 50

function PlaceBarrelsWestCrateWall()
    local count = GetNamedPointCount("BarrelsWestCrateWall")
    for i = 0, count - 1 do
      local x, y, z = GetNamedPoint("BarrelsWestCrateWall", i);
      CreateBarrel("Barrel", 25.0, 0.0, x - 48.0, y - 80.0)
      CreateBarrel("Barrel", 25.0, 0.0, x - 80.0, y - 20.0)
      CreateBarrel("Barrel", 25.0, 0.0, x - 54.0, y + 60.0)
    end
end

function PlaceBarrelsWestWall()
    local count = GetNamedPointCount("BarrelsWestWall")
    for i = 0, count - 1 do
      local x, y, z = GetNamedPoint("BarrelsWestWall", i);
      CreateBarrel("Barrel", 25.0, 0.0, x - 48.0, y - 30.0)
      CreateBarrel("Barrel", 25.0, 0.0, x - 40.0, y - 0.0)
      CreateBarrel("Barrel", 25.0, 0.0, x - 54.0, y + 24.0)
    end
end

function PlaceBarrelsEastWall()
    local count = GetNamedPointCount("BarrelsEastWall")
    for i = 0, count - 1 do
      local x, y, z = GetNamedPoint("BarrelsEastWall", i);
      CreateBarrel("Barrel", 25.0, 0.0, x + 42.0, y - 30.0)
      CreateBarrel("Barrel", 25.0, 0.0, x + 48.0, y - 0.0)
      CreateBarrel("Barrel", 25.0, 0.0, x + 44.0, y + 24.0)
    end
end

function PlaceBarrelsNorthWall()
    local count = GetNamedPointCount("BarrelsNorthWall")
    for i = 0, count - 1 do
      local x, y, z = GetNamedPoint("BarrelsNorthWall", i);
      CreateBarrel("Barrel", 25.0, 0.0, x - 30.0, y - 42.0)
      CreateBarrel("Barrel", 25.0, 0.0, x - 0.0,  y - 48.0)
      CreateBarrel("Barrel", 25.0, 0.0, x + 24.0, y - 44.0)
    end
end

function PlaceBarrelsFloor()
    local count = GetNamedPointCount("BarrelsFloor")
    for i = 0, count do
      local x, y, z = GetNamedPoint("BarrelsFloor", i);
      for j = 1, 3 do
        local atx = math.random() * 128.0 - 64.0
        local aty = math.random() * 128.0 - 64.0        
        CreateBarrel("Barrel", 25.0, 0.0, x + atx, y + aty)
      end
    end
end

function PlaceDoors(color)
    for j = 0, 4 - 1 do
      local doorName = "" .. color .. "DoorWay" .. j
      local count = GetNamedPointCount(doorName)
      for i = 0, count - 1 do
        local x, y, z = GetNamedPoint(doorName, i);
        CreateDoor(color, x, y, j)
      end
    end
end

function PlaceKeysLocation(color, location, xOffset, yOffset)
  local keyName = "Key" .. color
  local keyNameLocation = keyName .. location
  local count = GetNamedPointCount(keyNameLocation)
  print(keyNameLocation, " count = ", count)
  for i = 0, count - 1 do
    local x, y, z = GetNamedPoint(keyNameLocation, i);
    CreateItem(keyName, 1, true, x + xOffset, y + yOffset)
  end
end

function PlaceKeys(color)
  local direction  = { { "SE",   100, 100 }, { "NE",  100, -100 },
                       { "SW",  -100, 100 }, { "NW", -100, -100 } };
  for j = 1, #direction do
    local dirInfo = direction[j]
    PlaceKeysLocation(color, dirInfo[1], dirInfo[2], dirInfo[3])
  end
end

function PlaceBarrels()
  PlaceBarrelsFloor()
  PlaceBarrelsWestWall()
  PlaceBarrelsEastWall()
  PlaceBarrelsNorthWall()  
  PlaceBarrelsWestCrateWall()
end

function CreateMap()

  PlaceDoors("Red")
  PlaceDoors("Blue")
  PlaceDoors("White")
  
  PlaceKeys("Red")
  PlaceKeys("Blue")
  
  for i = 1, ENEMIES_MAX do  
    CreateRandomEnemy()    
  end
  
  for i = 1, LOOT_MAX do
    CreateRandomLootRandom()
  end
  
  PlaceBarrels()  
end