print "Lua:createmap.lua"

ENEMIES_MAX                      = 2000
MOVABLES_MAX                     = 1000
LOOT_MAX                         = 50
TILE_ENEMY_SPAWN_COUNT           = 50
TILE_ENEMY_SPAWN_DIFFICULT_COUNT = 30

LEVELS_SPAWN = {
    { level=1,  enemies_per_tile=3,                      enemies_difficult_per_tile=1,                                enemies_max=3,           enemies_actual=0, loot_weight=1.0 },
    { level=2,  enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=1.0 },
    { level=3,  enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=1.0 },
    { level=4,  enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=1.0 },
    { level=5,  enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=0.9 },
    { level=6,  enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=0.8 },
    { level=7,  enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=0.7 },
    { level=8,  enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=0.6 },
    { level=9,  enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=0.5 },
    { level=10, enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=0.4 },
    { level=11, enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=0.3 },
    { level=12, enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=0.2 },
    { level=13, enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=0.2 },
    { level=14, enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=0.2 },
    { level=15, enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=0.2 },
    { level=16, enemies_per_tile=TILE_ENEMY_SPAWN_COUNT, enemies_difficult_per_tile=TILE_ENEMY_SPAWN_DIFFICULT_COUNT, enemies_max=ENEMIES_MAX, enemies_actual=0, loot_weight=0.2 },
}

function TooManyMovables()
  if GetMovableCount() >= MOVABLES_MAX then
    return true
  end
  return false
end

function MakeRandomSeed()
  return math.random(0, 32768)
end

function GetTileEnemySpawnCount(difficult)
  spawn = LEVELS_SPAWN[LEVEL]
  if difficult then
    return spawn.enemies_difficult_per_tile
  else
    return spawn.enemies_per_tile
  end
end

function SpawnEnemiesAndLootAtTile(x, y, difficult)
  if x and y then
    print("SpawnEnemies")
    local count = GetTileEnemySpawnCount(difficult)
    for i = 1, count do
      if TooManyMovables() or LEVELS_SPAWN[LEVEL].enemies_actual >= LEVELS_SPAWN[LEVEL].enemies_max then
        break
      end
      CreateRandomEnemyAtTile(x, y, difficult)
      LEVELS_SPAWN[LEVEL].enemies_actual = LEVELS_SPAWN[LEVEL].enemies_actual + 1
    end
    local lootCount = math.random(1, 2)
    for i = 1, lootCount do
      if TooManyMovables() then
        break
      end
      CreateRandomLootAtTile(x, y)
    end
  end
end

function BarrelLikelihood()
  --return math.random(3) < 2
  return true
end

function CreateBarrelAndSerialize(x, y, seed)
  local barrel = CreateBarrel("Barrel", 25.0, 0.0, x, y, seed)
  if IS_MULTIPLAYER then
    local serializeCreated = SerializeCharacter(barrel)
    Rpc(DeserializeCharacter, serializeCreated)
  end
  return barrel
end

function CreateBarrelsWestCrateWallAtTile(x, y)
  if x and y then
    print("CreateBarrelsWCrate")
    x = x * 256
    y = y * 256
    if BarrelLikelihood() then CreateBarrelAndSerialize(x - 48.0, y - 80.0, MakeRandomSeed()) end
    if BarrelLikelihood() then CreateBarrelAndSerialize(x - 80.0, y - 20.0, MakeRandomSeed()) end
    if BarrelLikelihood() then CreateBarrelAndSerialize(x - 54.0, y + 60.0, MakeRandomSeed()) end
  end
end

function CreateBarrelsWestWallAtTile(x, y)
  if x and y then
    print("CreateBarrelsW")
    x = x * 256
    y = y * 256  
    if BarrelLikelihood() then CreateBarrelAndSerialize(x - 48.0, y - 80.0, MakeRandomSeed()) end
    if BarrelLikelihood() then CreateBarrelAndSerialize(x - 80.0, y - 20.0, MakeRandomSeed()) end
    if BarrelLikelihood() then CreateBarrelAndSerialize(x - 54.0, y + 60.0, MakeRandomSeed()) end
  end
end

function CreateBarrelsEastWallAtTile(x, y)
  if x and y then
    print("CreateBarrelsE")
    x = x * 256
    y = y * 256  
    if BarrelLikelihood() then CreateBarrelAndSerialize(x + 42.0, y - 30.0, MakeRandomSeed()) end
    if BarrelLikelihood() then CreateBarrelAndSerialize(x + 48.0, y -  0.0, MakeRandomSeed()) end
    if BarrelLikelihood() then CreateBarrelAndSerialize(x + 44.0, y + 24.0, MakeRandomSeed()) end
  end
end

function CreateBarrelsNorthWallAtTile(x, y)
  if x and y then
    print("CreateBarrelsN")
    x = x * 256
    y = y * 256  
    if BarrelLikelihood() then CreateBarrelAndSerialize(x - 30.0, y - 42.0, MakeRandomSeed()) end
    if BarrelLikelihood() then CreateBarrelAndSerialize(x -  0.0, y - 48.0, MakeRandomSeed()) end
    if BarrelLikelihood() then CreateBarrelAndSerialize(x + 24.0, y - 44.0, MakeRandomSeed()) end
  end
end

function CreateBarrelsFloorAtTile(x, y)
  if x and y then
    print("CreateBarrelsF")  
    x = x * 256
    y = y * 256  
    if BarrelLikelihood() then CreateBarrelAndSerialize(x - 30.0, y - 42.0, MakeRandomSeed()) end
    if BarrelLikelihood() then CreateBarrelAndSerialize(x -  0.0, y - 48.0, MakeRandomSeed()) end
    if BarrelLikelihood() then CreateBarrelAndSerialize(x + 24.0, y - 44.0, MakeRandomSeed()) end
  end
end

function OnFirstVisitListener(infoString, luaUserData)
  if IS_CLIENT then 
    Rpc(Visit, GetRenderRange())
    return
  end
  
  local spawn = "Spawn"
  local spawnDifficult = "SpawnDifficult"
  if StartsWith(infoString, spawnDifficult) then
    local x, y = ParseNumbers(infoString, 2, #spawnDifficult)
    SpawnEnemiesAndLootAtTile(x, y, true)
  elseif StartsWith(infoString, spawn) then
    local x, y = ParseNumbers(infoString, 2, #spawn)
    SpawnEnemiesAndLootAtTile(x, y, false)
  end
end

function OnTrueFirstVisitListener(infoString, luaUserData)
  if IS_CLIENT then return end
  
  local barrelsWCW = "BarrelsWestCrateWall"
  local barrelsWW  = "BarrelsWestWall"
  local barrelsEW  = "BarrelsEastWall"
  local barrelsNW  = "BarrelsNorthWall"  
  local barrelsF   = "BarrelsFloor"
  
  if StartsWith(infoString, barrelsWCW) then
    local x, y = ParseNumbers(infoString, 2, #barrelsWCW)
    CreateBarrelsWestCrateWallAtTile(x, y)
  elseif StartsWith(infoString, barrelsWW) then
    local x, y = ParseNumbers(infoString, 2, #barrelsWW)
    CreateBarrelsWestWallAtTile(x, y)
  elseif StartsWith(infoString, barrelsEW) then
    local x, y = ParseNumbers(infoString, 2, #barrelsEW)
    CreateBarrelsEastWallAtTile(x, y)    
  elseif StartsWith(infoString, barrelsNW) then
    local x, y = ParseNumbers(infoString, 2, #barrelsNW)
    CreateBarrelsNorthWallAtTile(x, y)
  elseif StartsWith(infoString, barrelsF) then
    local x, y = ParseNumbers(infoString, 2, #barrelsF)
    CreateBarrelsFloorAtTile(x, y)
  end  
end

function PlaceBarrelsWestCrateWall()
    local count = GetNamedPointCount("BarrelsWestCrateWall")
    for i = 0, count - 1 do
      local x, y, z = GetNamedPoint("BarrelsWestCrateWall", i);
      if BarrelLikelihood() then CreateBarrel("Barrel", 25.0, 0.0, x - 48.0, y - 80.0, MakeRandomSeed()) end
      if BarrelLikelihood() then CreateBarrel("Barrel", 25.0, 0.0, x - 80.0, y - 20.0, MakeRandomSeed()) end
      if BarrelLikelihood() then CreateBarrel("Barrel", 25.0, 0.0, x - 54.0, y + 60.0, MakeRandomSeed()) end
    end
end

function PlaceBarrelsWestWall()
    local count = GetNamedPointCount("BarrelsWestWall")
    for i = 0, count - 1 do
      local x, y, z = GetNamedPoint("BarrelsWestWall", i);
      if BarrelLikelihood() then CreateBarrel("Barrel", 25.0, 0.0, x - 48.0, y - 30.0, MakeRandomSeed()) end
      if BarrelLikelihood() then CreateBarrel("Barrel", 25.0, 0.0, x - 40.0, y -  0.0, MakeRandomSeed()) end
      if BarrelLikelihood() then CreateBarrel("Barrel", 25.0, 0.0, x - 54.0, y + 24.0, MakeRandomSeed()) end
    end
end

function PlaceBarrelsEastWall()
    local count = GetNamedPointCount("BarrelsEastWall")
    for i = 0, count - 1 do
      local x, y, z = GetNamedPoint("BarrelsEastWall", i);
      if BarrelLikelihood() then CreateBarrel("Barrel", 25.0, 0.0, x + 42.0, y - 30.0, MakeRandomSeed()) end
      if BarrelLikelihood() then CreateBarrel("Barrel", 25.0, 0.0, x + 48.0, y -  0.0, MakeRandomSeed()) end
      if BarrelLikelihood() then CreateBarrel("Barrel", 25.0, 0.0, x + 44.0, y + 24.0, MakeRandomSeed()) end
    end
end

function PlaceBarrelsNorthWall()
    local count = GetNamedPointCount("BarrelsNorthWall")
    for i = 0, count - 1 do
      local x, y, z = GetNamedPoint("BarrelsNorthWall", i);
      if BarrelLikelihood() then CreateBarrel("Barrel", 25.0, 0.0, x - 30.0, y - 42.0, MakeRandomSeed()) end
      if BarrelLikelihood() then CreateBarrel("Barrel", 25.0, 0.0, x - 0.0,  y - 48.0, MakeRandomSeed()) end
      if BarrelLikelihood() then CreateBarrel("Barrel", 25.0, 0.0, x + 24.0, y - 44.0, MakeRandomSeed()) end
    end
end

function PlaceBarrelsFloor()
    local count = GetNamedPointCount("BarrelsFloor")
    for i = 0, count do
      local x, y, z = GetNamedPoint("BarrelsFloor", i);
      for j = 1, 3 do
        local atx = math.random() * 128.0 - 64.0
        local aty = math.random() * 128.0 - 64.0        
        if BarrelLikelihood() then CreateBarrel("Barrel", 25.0, 0.0, x + atx, y + aty, MakeRandomSeed()) end
      end
    end
end

DOOR_LIST = {}

function GetDoorHash(x, y)
  return tostring(math.floor(x / 10) + math.floor(y / 10) * 65536)
end

function PlaceDoors(color)
  for j = 0, 4 - 1 do
    local doorName = "" .. color .. "DoorWay" .. j
    local count = GetNamedPointCount(doorName)
    for i = 0, count - 1 do
      local x, y, z = GetNamedPoint(doorName, i);
      local door = CreateDoor(color .. "Door", x, y, j, MakeRandomSeed())
      DOOR_LIST[GetDoorHash(x, y)] = { x, y, door, 'Locked' }
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
    CreateItem(keyName, 1, true, x + xOffset, y + yOffset, MakeRandomSeed())
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

function PlaceAmaranthRing()
  local count = GetNamedPointCount("AmaranthRing")
  for i = 0, count - 1 do
    local x, y, z = GetNamedPoint("AmaranthRing", i);
    CreateItem("Ring", 1, true, x - 100, y - 100, MakeRandomSeed())
  end
end

function PlaceBarrels()
  PlaceBarrelsFloor()
  PlaceBarrelsWestWall()
  PlaceBarrelsEastWall()
  PlaceBarrelsNorthWall()  
  PlaceBarrelsWestCrateWall()
end

function PlaceGeneratorGhosts()
  LOCAL_GAME_STATE.ActiveGeneratorGhosts = 0
  local count = GetNamedPointCount("GeneratorWest")
  for i = 0, count - 1 do
    local x, y, z = GetNamedPoint("GeneratorWest", i);
    CreateGeneratorGhost("GeneratorGhost", 1000.0, 0.0, x + 72.0, y + 32.0, MakeRandomSeed())
    CreateGeneratorGhost("GeneratorGhost", 1000.0, 0.0, x + 72.0, y - 48.0, MakeRandomSeed())
    LOCAL_GAME_STATE.ActiveGeneratorGhosts = LOCAL_GAME_STATE.ActiveGeneratorGhosts + 2
  end
  local count = GetNamedPointCount("GeneratorEast")
  for i = 0, count - 1 do
    local x, y, z = GetNamedPoint("GeneratorEast", i);
    CreateGeneratorGhost("GeneratorGhost", 1000.0, 0.0, x -   0.0, y + 32.0, MakeRandomSeed())
    CreateGeneratorGhost("GeneratorGhost", 1000.0, 0.0, x -   0.0, y - 48.0, MakeRandomSeed())
    CreateGeneratorGhost("GeneratorGhost", 1000.0, 0.0, x + 128.0, y - 48.0, MakeRandomSeed())
    CreateGeneratorGhost("GeneratorGhost", 1000.0, 0.0, x + 128.0, y + 32.0, MakeRandomSeed())
    LOCAL_GAME_STATE.ActiveGeneratorGhosts = LOCAL_GAME_STATE.ActiveGeneratorGhosts + 4
  end  
end

function PlaceGuides()
  local count = GetNamedPointCount("BlueTent")
  for i = 0, count - 1 do
    local x, y, z = GetNamedPoint("BlueTent", i);
    CreateGuide("Guide", 10000000.0, 0.0, x - 128.0, y - 128.0, MakeRandomSeed())
  end
end

function PlaceStores()
  local count = GetNamedPointCount("RedTent")
  for i = 0, count - 1 do
    local x, y, z = GetNamedPoint("RedTent", i);
    CreateStore("Store", 10000000.0, 0.0, x - 128.0, y - 128.0, MakeRandomSeed())
  end
end

function PlaceSavePoints()
  local count = GetNamedPointCount("Market")
  for i = 0, count - 1 do
    local x, y, z = GetNamedPoint("Market", i);
    CreateSavePoint("SavePoint", 10000000.0, 0.0, x - 128.0, y - 128.0, MakeRandomSeed())
  end
end

function PlaceBoss1()
  local count = GetNamedPointCount("Boss1")
  for i = 0, count - 1 do
    local x, y, z = GetNamedPoint("Boss1", i);
    CreateQuinotaur1("Quinotaur1", 25000, 4.0, x - 128.0, y - 128.0, 0.0, MakeRandomSeed())
  end
end

function PlaceBoss2()
  local count = GetNamedPointCount("Boss2")
  for i = 0, count - 1 do
    local x, y, z = GetNamedPoint("Boss2", i);
    CreateArachnarex("Arachnarex", 5000, 4.0, x - 128.0, y - 128.0, 0.0, MakeRandomSeed())
  end
end

function CreatePersistentMap()

  PlaceDoors("Red")
  PlaceDoors("Blue")
  PlaceDoors("White")
  
  PlaceGuides()
  PlaceSavePoints()
  PlaceStores()
  
  PlaceGeneratorGhosts()  

end

function CreateMap()

  PlaceKeys("Red")
  PlaceKeys("Blue")

  PlaceAmaranthRing()
  
  PlaceBoss1()
  PlaceBoss2()  
  
end
