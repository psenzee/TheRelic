print "Lua:load.lua"

--[[

Loading is complicated so it has its own file.

The public API for this file is -

UnloadGame()
------------

Unloads a game over several frames.  Only works from a starting game state of GAMESTATE_INGAME.
At completion sets GAMESTATE_UNLOADED and goes through in-between states (GAMESTATE_UNLOADING, GAMESTATE_UNLOADING_PLAYERS)

LoadGame(filename)
------------------

Loads a save game from a file.  Also unloads a current game if present and loads the appropriate level.

NewGame(level)
--------------

Creates a new game at the given level (typically 1).  Also unloads a current game if present.

DeserializeGame(data)
------------------

Loads a game from a chunk of compressed data, especially for use over the network - joining a game.  
Also unloads a current game if present and loads the appropriate level.

]]--

----------------------------------------------------------------------------------------------------------------------------

-- this is called by the native "EndGame"
function OnEndGameComplete()
  if GetGameStateId() == GAMESTATE_UNLOADING then
    DeferByTimeMs(10, "DestroyPlayers") -- if we made the player table a C++ abstraction, we could avoid many of these shenanigans
  end
end

function DestroyPlayers()
  print("Lua:DestroyPlayers")
  SetGameStateId(GAMESTATE_UNLOADING_PLAYERS)  
  SetPlayer(nil)
  local players = 0
  for i = 1, #PLAYERS do
    local player = PLAYERS[i]
    if player ~= nil then
      PLAYERS[i] = nil
      players = players + 1
      DestroyCharacter(player)
    end
  end
  if players == 0 then
    SetGameStateId(GAMESTATE_UNLOADED)
  end
end

function OnDestroyCharacterComplete(id)
  if GetGameStateId() == GAMESTATE_UNLOADING_PLAYERS then
    local finished = true
    for i = 1, #PLAYERS do  
      if PLAYERS[i] ~= nil then
        finished = false
      end
    end
    if finished then
      SetGameStateId(GAMESTATE_UNLOADED)
    end
  end
end

function UnloadGame()
  if GetGameStateId() == GAMESTATE_INGAME then
    SetGameStateId(GAMESTATE_UNLOADING)
    DeferByTimeMs(10, "EndGameSession")
  end
end

function OnStartGameComplete()
  SetGameStateId(GAMESTATE_STARTED)
end

function StartGameSessionAndDeserializePlayer(level, save)
  DeferByGameStateId(GAMESTATE_STARTED, "FinishDeserializePlayer", save)
  SetGameStateId(GAMESTATE_LOADING)  
  StartGameSession(level)
end

function DeserializeGameLevel(level, save)
  print("Lua:DeserializeGameLevel")
  if GetGameStateId() == GAMESTATE_INGAME then
    UnloadGame()
    DeferByGameStateId(GAMESTATE_UNLOADED, "StartGameSessionAndDeserializePlayer", level, save)
  else
    DeferByTimeMs(10, "StartGameSessionAndDeserializePlayer", level, save)
  end
end

function OnLoadGameComplete()
  SetGameStateId(GAMESTATE_INGAME)
  MessageRise("\\#80ff80Game Loaded\\#ffffff ", 0)
end  

function LoadGame(filename)
  local path = MakeReadWritePath(filename)
  print ("Loading character data from '" .. path .. "'")
  local f = io.open(path, 'rb')
  local data = f:read("*all")
  f:close()
  DeferByGameStateId(GAMESTATE_DESERIALIZED, "OnLoadGameComplete")
  DeserializeGame(data)
end

function DeserializeGame(data)
  print ("Deserializing character data")
  data = Decompress(data)
  local save = Deserialize(data)
  local gameLevel = save.gameLevel -- we need to figure out what to do here since we need to know the level in order to load it, before we set the character stuff (because they will be destroyed, then created)
  DeserializeGameLevel(gameLevel, save)
end

function FinishDeserializePlayer(save)
  local c = GetPlayer()
  print ("Deserializing character data")
  local gameLevel = save.gameLevel -- we need to figure out what to do here since we need to know the level in order to load it, before we set the character stuff (because they will be destroyed, then created)
  -- load level
  c:SetPosition(save.position[1], save.position[2], save.position[3])
  SetVisitedData(save.visited)  
  SetAttributeTable(save.attributes, c)  
  UpdatePortalsFromList(save.portals)
  LOCAL_GAME_STATE = save.localGameState
  local cdata = c:data()
  for k, v in pairs(cdata) do
    cdata[k] = nil    
  end
  deepcopyinto(save.character, cdata)
  RebuildInventoryScreen(c)
  UpdateDoorListFrom(save.doors)
  UpdateDoorsFromList()
  DeserializeCharacterList(save.characters)
  SetGameStateId(GAMESTATE_DESERIALIZED)
end

function EnableMap()
  ClearButtonPresses()
  MAP_ENABLED = true
end

function _EnableMapAndSave()
  SetIndicatorVisibility('SaveButton', true)
  EnableMap()
end

function OnNewGameComplete(level)
  SetGameStateId(GAMESTATE_INGAME)
--MessageRise("\\#80ff80New Game\\#ffffff ", 0)
  SetIndicatorVisibility('SaveButton', false)
  ShowIndicator('SaveButton')
  if not IsMultiplayer() then
--    DeferByTimeMs(2000, "StartSequence", START_SEQUENCE)
  else
    local id = GetPlayer():GetId() + 1
print ("** SETTING data for id " .. id)
    Rpc(SetNameForPlayerIndicator, GetPlatformDeviceName(), id)
    SetNameForPlayerIndicator(GetPlatformDeviceName(), id)    
  end
  DeferByTimeMs(2500, "_EnableMapAndSave")
end

function NewGame(level)
  print("Lua:NewGame")
  MAP_ENABLED = false -- don't enable the map until the start sequence has completed
  DeferByGameStateId(GAMESTATE_STARTED, "OnNewGameComplete", level)
  if GetGameStateId() == GAMESTATE_INGAME then
    UnloadGame()
    DeferByGameStateId(GAMESTATE_UNLOADED, "StartGameSession", level)
  else
    DeferByTimeMs(10, "StartGameSession", level)
  end
end
