print "Lua:Starting Game Script"
print "Lua:game.lua"

dofile (MakeReadOnlyPath("common.lua"))
dofile (MakeReadOnlyPath("serialize.lua"))
dofile (MakeReadOnlyPath("inventory.lua"))
dofile (MakeReadOnlyPath("fx.lua"))
dofile (MakeReadOnlyPath("characters.lua"))
dofile (MakeReadOnlyPath("player.lua"))
dofile (MakeReadOnlyPath("drifter.lua"))
dofile (MakeReadOnlyPath("enemies.lua"))
dofile (MakeReadOnlyPath("loot.lua"))
dofile (MakeReadOnlyPath("door.lua"))
dofile (MakeReadOnlyPath("createmap.lua"))
dofile (MakeReadOnlyPath("teleporter.lua"))
dofile (MakeReadOnlyPath("multiplayer.lua"))
dofile (MakeReadOnlyPath("ui.lua"))
dofile (MakeReadOnlyPath("rpg.lua"))
dofile (MakeReadOnlyPath("trailer.lua"))
dofile (MakeReadOnlyPath("relic-dictionary.lua"))

function GameInitialize(instance)

  print("Lua:GameInitialize")
  
  CreatePlayers(4)
  
  SetCameraFovDegrees(30)

  FindTeleportTargets()

  if server then
    CreateMap()
  end
  
  SetBaseDepthScale(1.0)  
  SetDefaultMovableRadius(characterRadius)
  
  StartMultiplayer(server)
  StartSerialization()
  
  ChangeGameMode(INTRO)
  
end

local lightingType = 0


function TestRpc(method, ...)
local packed = PackRpc(0, method, arg)
print ('>> packed message length ' .. tostring(#packed) .. '; unpacking for test ..')
print ('>> unpacked ' .. tostring(ReadRpc(packed)))
end

function UpdateGame(instance)

--  TestRpc(ReceiveCharacters, SerializeCharacterList())

  UpdateMultiplayer()

  if ButtonPressed(3) then
--  lightingType = lightingType + 1
--  SetDefaultLightingType(lightingType)
    ChangeGameMode((GameMode + 1) % MAX_GAME_MODE)
  end
  
  OccasionallyPrintInventory()
  
  UpdateTeleportation()

  if not server then
    GetCharactersFromServer()
  end
 
end
