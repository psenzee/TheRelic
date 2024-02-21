print "Lua:Starting Game Script"
print "Lua:game.lua"

dofile (MakeReadOnlyPath("common.lua"))
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

function HostGame()
    Multiplayer_StartServer("Paul'sServer")
end

function JoinGame()
    Multiplayer_StartClient("Paul'sClient")
    Multiplayer_Join("127.0.0.1")
end

local server = false

function StartMultiplayer()
  if server then
    HostGame()
  else
    JoinGame()
  end
end

function GameInitialize(instance)

  print("Lua:GameInitialize")
  
  SetCameraFovDegrees(30)
  CreateNewPlayer(512, 512)

  CreateMap()
  
  SetBaseDepthScale(1.0)  
  SetDefaultMovableRadius(characterRadius)
  
  --server = true
  StartMultiplayer()

end

local lightingType = 0

function UpdateGame(instance)

  if ButtonPressed(3) then
    lightingType = lightingType + 1
    SetDefaultLightingType(lightingType)
  end
  
  OccasionallyPrintInventory()
  
  UpdateTeleportation()

end
