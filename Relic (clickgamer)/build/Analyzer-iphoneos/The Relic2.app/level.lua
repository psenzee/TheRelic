print "Lua:level.lua"

dofile (MakeReadOnlyPath("createmap.lua"))
dofile (MakeReadOnlyPath("teleporter.lua"))

LEVEL = 1

local START_AT_NONE       = 0
local START_AT_START      = 1
local START_AT_UPSTAIRS   = 2
local START_AT_DOWNSTAIRS = 3

local START_AT = START_AT_NONE

local IGNORE_NEXT_ENTER_UPSTAIRS   = false
local IGNORE_NEXT_ENTER_DOWNSTAIRS = false

local TOTAL_LEVELS = 13

DIALOG_ON = false
DIALOG    = ""

IS_LOADING_SAVED_LEVEL = false

function SetDialog(dialog)
  DIALOG_ON = true
  DIALOG = dialog
end

function SetStoryDialogById(id)
--  SetDialog(StoryDialog[id])
end

function ClearDialog()
  DIALOG_ON = false
  DIALOG = ""  
end

function CompleteLoadLevel(level)
  IS_LOADING = true
  UnloadSequenceTextures()  
  LoadLevel(level)
  SetPaused(true)
  ClearDialog()
  StartPlayerGameStateLevel(level)
end

function AscendNextLevel()
  LEVEL = GetLevelNumber() + 1
  if LEVEL > TOTAL_LEVELS then
    LEVEL = 1
  end
  CompleteLoadLevel(LEVEL)
  START_AT = START_AT_DOWNSTAIRS
end

function DescendNextLevel()
  LEVEL = GetLevelNumber() - 1
  if LEVEL < 1 then
    LEVEL = TOTAL_LEVELS
  end
  CompleteLoadLevel(LEVEL)
  START_AT = START_AT_UPSTAIRS  
end

function UpStairsListener(infoString, luaUserData)
  if infoString ~= "Enter" then
    return
  end
  if IGNORE_NEXT_ENTER_UPSTAIRS then
    IGNORE_NEXT_ENTER_UPSTAIRS = false
    return
  end
  print ("** Event Listened: ", infoString, luaUserData)
  if IsMultiplayer() then -- we also need to check that everyone is here..
    ShowLevelExitWaitMenu()  
  else
    IS_IN_STORE = false
    ShowLevelCompleteMenu()
    --AscendNextLevel()
  end
  IGNORE_NEXT_ENTER_DOWNSTAIRS = true
end

function DownStairsListener(infoString, luaUserData)
  if infoString ~= "Enter" then
    return
  end
  if IGNORE_NEXT_ENTER_DOWNSTAIRS then
    IGNORE_NEXT_ENTER_DOWNSTAIRS = false
    return
  end  
  print ("** PreviousLevel: ", infoString, luaUserData)
  ShowNoDescendMenu()
  local xp, yp, zp = GetPlayer():GetPosition()
  GetPlayer():SetPosition(xp - 150, yp, zp)
  --DescendNextLevel()
  IGNORE_NEXT_ENTER_UPSTAIRS = true
end

function GetSpecificTag(tag, index)
  return tag .. "" .. index .. "L" .. GetLevelNumber()
end

REGISTERED_LISTENERS = {}

function RegisterLocationListener(tag, i, radius, listenerName, listenerData)
  local xt, yt, zt = GetNamedPoint(tag, i - 1)
  local name = GetSpecificTag(tag, i)
  SetLocationEvent(name, xt, yt, zt, radius)  
  REGISTERED_LISTENERS[name] = RegisterListener(name, listenerName, listenerData)
end    

function UnregisterLocationListener(tag, i)
  local name = GetSpecificTag(tag, i)
  RemoveLocationEvent(name)
  local listener = REGISTERED_LISTENERS[name]
  if listener ~= nil then
    UnregisterListener(listener)
    REGISTERED_LISTENERS[name] = nil
  end
end

function RegisterDialogForAll(tag, radius)
  local count  = GetNamedPointCount(tag)
  for i = 1, count do
    RegisterLocationListener(tag, i, 200.0, "DialogListener", { dialogId = GetSpecificTag(tag, i) .. "_1" })
  end
end

function UnregisterDialogForAll(tag)
  local count  = GetNamedPointCount(tag)
  for i = 1, count do
    UnregisterLocationListener(tag, i)
  end
end

DID_REGISTER_LISTENERS = false

-- Called by C++
function LevelLoad()
  
  FindTeleportTargets()
  CreatePersistentMap()
  
  if not IS_LOADING_SAVED_LEVEL then
    CreateMap()
  end
  
  DID_REGISTER_LISTENERS = false
  IS_LOADING = nil
  DoFlash(2000)
  ClearDialog()
  SetPaused(false)
  GAME_ENDED = false
end

function UpdateStartPoint()
  if START_AT ~= START_AT_NONE then
    local xt, yt, zt = GetNamedPoint("Start1", 0)
    if START_AT == START_AT_UPSTAIRS then
      xt, yt, zt = GetNamedPoint("UpStairs", 0)
    elseif START_AT == START_AT_DOWNSTAIRS then
      xt, yt, zt = GetNamedPoint("DownStairs", 0)
      xt = xt - 150
    end
    if GetPlayer() ~= nil then GetPlayer():SetPosition(xt, yt, zt) end
    START_AT = START_AT_NONE
  end
end

function LevelUpdate()

  if IS_LOADING == true then
    return
  end
  
  if DID_REGISTER_LISTENERS == false then
    LEVEL = GetLevelNumber()
    RegisterLocationListener("UpStairs",   0, 64.0, "UpStairsListener",   {})
    RegisterLocationListener("DownStairs", 0, 64.0, "DownStairsListener", {})  
    DID_REGISTER_LISTENERS = true  
    
    local sequences =
    {
      START_SEQUENCE,
      --QUINOTAUR_DESCRIBE_SEQUENCE_1,
      nil,
      --RELIC_WEAPON_SEQUENCE,
      nil,
      --CATACLYSM_SEQUENCE,
      nil,
      RELIC_WEAPON_SEQUENCE,
      --LAST_LEVEL_SEQUENCE,
      nil,
      nil,      nil,      nil,      nil,      LAST_LEVEL_SEQUENCE,      nil,      nil,      nil,
      nil,      nil,      nil,      nil,      nil,      nil,      nil,      nil,                                                                                    
      -- ..
    }
    if sequences[LEVEL] ~= nil and not IsMultiplayer() and not LOCAL_GAME_STATE.LevelsVisited[LEVEL] then
      LOCAL_GAME_STATE.LevelsVisited[LEVEL] = true
      DeferByTimeMs(2000, "StartSequence", sequences[LEVEL])
    end
    DeferByTimeMs(1000, "ShowAchievement", "Level" .. LEVEL)
  end
  
  UpdateStartPoint()
  UpdateTeleportation()
  
end

function LevelUnload()
  if DID_REGISTER_LISTENERS == true then
    UnregisterLocationListener("UpStairs",   0)
    UnregisterLocationListener("DownStairs", 0)
    DID_REGISTER_LISTENERS = false
  end
  -- clean up this level
  ClearPortals()
  DESTROYED_GENERATORS = {}
  UnloadLevelTextures()
end

function UnloadLevelTextures()
  UnloadSequenceTextures()
  UnloadTexture("FutureFloor")
  UnloadTexture("FloorLava5")
  UnloadTexture("WoodFloor")
  UnloadTexture("FutureTop")
  UnloadTexture("AlienArch")
  UnloadTexture("Floor_Ice")
  UnloadTexture("Top2")
  UnloadTexture("WallTypesB_11")
  UnloadTexture("WallTypesA_11")
  UnloadTexture("WallTypesA_13")
  UnloadTexture("WallTypesB_13")
  UnloadTexture("WallTypesB_5b")  
  UnloadTexture("WallTypesA_5b")
  UnloadTexture("WallTypesB_9")
  UnloadTexture("WallTypesA_9")
  UnloadTexture("FloorStone3_1sm")
  UnloadTexture("FloorStone3_1smAlt")
  UnloadTexture("WallTypesA_Amaranth")
  UnloadTexture("OriginalArch")
  UnloadTexture("Baked_Arch0_5")
  UnloadTexture("Baked_UpStairsL_11")
  UnloadTexture("Baked_UpStairsL_13")
  UnloadTexture("Baked_UpStairsL_18")
  UnloadTexture("Baked_UpStairsL_9")
  UnloadTexture("StoneGrassFloor3")
  UnloadTexture("StoneGrassFloor4")
  UnloadTexture("FloorLava5Alt")
  UnloadTexture("Floor_Ice2")
  UnloadTexture("Floor_IceAlt")
  UnloadTexture("Floor_IceAlt2")
  UnloadTexture("Floor_BrownStone")
  UnloadTexture("Floor_BrownStoneAlt")
  UnloadTexture("Baked_DownStairs_11")
  UnloadTexture("Baked_DownStairs_13")
  UnloadTexture("Baked_DownStairs_18")
  UnloadTexture("Baked_DownStairs_9")
  UnloadTexture("Stone18")
  UnloadTexture("Stone18Alt")
  UnloadTexture("FloorLava5")
  UnloadTexture("FloorLava5Alt")
  UnloadTexture("FloorStone3_1sm")
  UnloadTexture("FloorStone3_1smAlt")
  UnloadTexture("Stone7")
  UnloadTexture("Stone7Alt")
  UnloadTexture("Stone7Alt2")  
  UnloadTexture("Calcada3")
  UnloadTexture("Calcada3Alt")
  UnloadTexture("StoneGrassFloor3")
  UnloadTexture("StoneGrassFloor4")
  UnloadTexture("Brick7")
  UnloadTexture("Brick7Alt")
  UnloadTexture("TileFloor10")
  UnloadTexture("TileFloor10Alt")
  UnloadTexture("WoodFloor2")
  UnloadTexture("WoodFloor2Alt")
  UnloadTexture("TileFloor16")
  UnloadTexture("TileFloor16Alt")
  UnloadTexture("TileFloor16Alt2")
  UnloadTexture("TileFloor18")
  UnloadTexture("TileFloor18Alt")
  UnloadTexture("Stone12")
  UnloadTexture("Stone12Alt")
  UnloadTexture("TileFloor10")
  UnloadTexture("TileFloor10Alt")
  UnloadTexture("Stone22")
  UnloadTexture("Stone22Alt")
  UnloadTexture("Stone18")
  UnloadTexture("Stone18Alt")
  UnloadTexture("TileFloor5")
  UnloadTexture("TileFloor5Alt")
  
  UnloadTexture("crystal")
  
  RemoveTileSet("dl-stonewall.xml")
  RemoveTileSet("dl-stonewall-1.xml")
  RemoveTileSet("dl-stonewall-2.xml")
  RemoveTileSet("dl-stonewall-3.xml")
  RemoveTileSet("dl-stonewall-4.xml")
  RemoveTileSet("dl-icewall.xml")
  RemoveTileSet("dl-icewall-2.xml")  
  RemoveTileSet("dl-icewall-3.xml")   
  RemoveTileSet("dl-future.xml")
  RemoveTileSet("dl-future-2.xml")  
  RemoveTileSet("dl-amaranth.xml")
  RemoveTileSet("dl-amaranth-2.xml")  
end