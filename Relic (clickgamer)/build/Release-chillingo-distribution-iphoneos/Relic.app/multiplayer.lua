print "Lua:multiplayer.lua"

TMP_ADDRESS = "192.168.1.102"
IS_JOINING = false
IS_MULTIPLAYER = false
IS_CLIENT = false
IS_STARTING_CLIENT = false
CHARACTER_POSITION_PACKETS = { nil, nil, nil, nil }
ACCEPTED = false

PLAYERS = {}
CLIENTS = { 0 }
PLAYERS_NAME = { "", "", "", "" }
--RPC_BROADCAST = 127
RPC_BROADCAST = 0

local ALREADY_CONNECTED = false

function SetMultiplayer(mp)
  IS_MULTIPLAYER = mp
end

function IsMultiplayer()
  return IS_MULTIPLAYER or IS_STARTING_CLIENT
end

function IsStartingClient()
  return IS_STARTING_CLIENT
end

function SetLocalPlayer(playerId, visible)
  local player = PLAYERS[playerId]
  if player == nil then
    return
  end
  SetPlayer(player)
  player:SetVisible(visible)
  for i = 1, #PLAYERS do
    local data = PLAYERS[i]:data()
    if data ~= nil then
      data.isLocalPlayer = false
    end
  end
  local data = player:data()  
  if data ~= nil then
    data.isLocalPlayer = true
  end
  RpcInitialize()
end

function CreatePlayers(count)
  for i = 1, count do
    local player = CreateNewPlayer(i)
    table.insert(PLAYERS, player)
    player:SetVisible(false)
  end
  --SetLocalPlayer(1, false)
  SetLocalPlayer(1, true)
end

function GetClosestPlayer(to)
  if to == nil then
    return nil
  end
  local closest = nil
  local distance = 100000000000
  for i = 1, #PLAYERS do
    local player = PLAYERS[i]
    if player ~= nil then
      local d = CharacterDistance(to, player)
      if d < distance then
        distance = d
        closest  = player
      end
    end
  end
  return closest
end

function StartMultiplayer(server)
end

function SetThisWaitingForOthers(waiting)
  local id = GetPlayer():GetId()
  SetWaitingForOthers(id, waiting)
  Rpc(SetWaitingForOthers, id, waiting)
end

PLAYER_WAITING_FOR_OTHERS = { false, false, false, false }

function ClearWaitingForOthers()
  PLAYER_WAITING_FOR_OTHERS = { false, false, false, false }
end

function GetCountWaitingForOthers(state)
  local count = 0
  for i = 1, 4 do
    if state == PLAYER_WAITING_FOR_OTHERS[i] then
      count = count + 1
    end
  end
  return count
end

function SetWaitingForOthers(index, waiting)
  PLAYER_WAITING_FOR_OTHERS[index] = waiting
  print("PLAYER_WAITING_FOR_OTHERS[" .. index .. "] = " .. tostring(waiting))
end

function UpdateWaitingForOthers()
  local waiting = PLAYER_WAITING_FOR_OTHERS[GetPlayer():GetId()]
  Rpc(SetWaitingForOthers, GetPlayer():GetId(), waiting)
end

function UpdateMultiplayer()
  RpcProcess()
  if not IS_CLIENT and GetCountWaitingForOthers(true) == Multiplayer_GetConnectionCount() + 1 then
    print("DONE WAITING")
    Rpc(LoadNextMPLevel)
    LoadNextMPLevel()
  end
end

function LoadNextMPLevel()
  -- $TODO THIS NEEDS TO WORK RIGHT
  ClearWaitingForOthers()
  ResumeFromInGame()
  AscendNextLevel()
end

function Echo(fromName, message)
  print (fromName .. ": " .. message)
end

-- RPC Implementation

-- Notes
-- need guaranteed messages
-- player ids are between 1-4 inclusive or zero for all

-- PUBLIC INTERFACE
-- 'void' RpcInitialize(<none>)  -- after initializing multiplayer, or if player id changes, called from SetLocalPlayer
-- 'void' RpcProcess(<none>)
-- 'void' RpcUser(toUserId, methodToCall, ...)
-- 'void' Rpc(methodToCall, ...) -- for all users

function RpcInitialize()
  if IsCharacterValid(GetPlayer()) then
    SetThisUserId(GetPlayer():GetId())
  end
end

function RpcProcess()
  while Sync_HasLuaMessage() do
    HandleRpc(Sync_PopLuaMessage())
  end
  while Sync_HasDataMessage() do
    HandleData(Sync_PopDataMessage())
  end  
end

DEBUG_FILE_NUMBER = 1

function RpcUser(toUserId, method, ...)
  local message = PackRpc(toUserId, method, arg)
  if #message > 5 * 1024 then
    print ('>> Sending large message of ' .. tostring(#message) .. ' bytes (writing to disk for examination)')
    local f = io.open('rpc' .. tostring(DEBUG_FILE_NUMBER) .. '.txt', 'wb')
    f:write(message)
    io.close(f)
    DEBUG_FILE_NUMBER = DEBUG_FILE_NUMBER + 1
  end
  Sync_SendLuaMessage(message)
--Sync_SendLuaMessage(PackRpc(toUserId, method, arg))
end

function Rpc(method, ...)
  RpcUser(RPC_BROADCAST, method, unpack(arg))
--Sync_SendLuaMessage(PackRpc(0, method, arg))
end

-- PRIVATE IMPLEMENTATION

function TestPrintMessage(message)
  print ("Received message '" .. message .. "'")
end

function HandleRpc(message)
  local success, fromUserId, method, arguments = ReadRpc(message)
  if success then
    method(unpack(arguments))
  end
  return success
end

function HandleData(message)
  local i = GetCharacterIdFromPositionPacket(message)
  CHARACTER_POSITION_PACKETS[i] = message
  return true
end

-------------------------

local charactersRequested = false

function GetCharactersFromServer()
  if not charactersRequested then
    print ">>> sending character list request to server"
    RpcUser(1, SendCharacters, GetThisUserId())
    charactersRequested = true  
  end  
end

function SendCharacters(to)
  print ">>> received SendCharacters request"
--RpcUser(to, ReceiveCharacters, SerializeCharacterList())
end

function ReceiveCharacters(chars)
  print ">>> receiving/deserializing characters.."
  DeserializeCharacterList(chars)
end

function SetAttribute(c, key, value)
  c:data()[key] = value
end

function SetAttribute_ById(cId, key, value)
  SetAttribute(GetCharacterById(cId), key, value)  
end

function MultiplayerSetAttribute(c, key, value)
  SetAttribute(c, key, value)
  Rpc(SetAttribute_ById, c:GetId(), key, value)
end

---------------------------------------------------

function StartWiFiServer()
  StartBonjour()
  Multiplayer_StartENetServer(GetPlatformDeviceName(), 70)
  IS_CLIENT = false
  IS_MULTIPLAYER = true  
end

function VerifyConnected()
  if not ACCEPTED then
    MultiplayerConnectFailed()
  end
end

function StartWiFiClient()
  IS_STARTING_CLIENT = true
  IS_CLIENT = true
  IS_MULTIPLAYER = true  
  StartWiFiPicker()
  DeferByTimeMs(10000, "VerifyConnected")
end

function StartBluetoothServer()
  Multiplayer_StartBluetoothServer(GetPlatformDeviceName())
  DeferByTimeMs(2000, "SetPaused", true)
  IS_CLIENT = false
  IS_MULTIPLAYER = true
end

function StartBluetoothClient()
  IS_STARTING_CLIENT = true
  StartBluetoothPicker()
  SetPaused(true)
  IS_CLIENT = true
  IS_MULTIPLAYER = true
end

function OnReceiveAddress(address, port)
  print("****** received address ", address, ":", tostring(port))
  if not Multiplayer_StartENetClient(GetPlatformDeviceName(), address, 70) then
    print("*****END GAME CONNECT FAILED******")
    EndGameConnectFailed()
  end
end

function OnReceiveBluetoothConnection()
  print("****** bluetooth connected ")
  if IsStartingClient() then
    Multiplayer_StartBluetoothClient(GetPlatformDeviceName())
  end
end

function MultiplayerConnectSucceeded()
  print "********Succeeded********"
  SetPaused(false)
end

function MultiplayerConnectCancelled()
  print "********Cancelled********"
  SetPaused(false)
  EndGameConnectFailed()  
end

function MultiplayerConnectFailed()
  print "********Failed********"
  SetPaused(false)
  EndGameConnectFailed()
end

function SetInGameState(id)
  SetGameStateId(GAMESTATE_INGAME)
  Rpc(SetNameForPlayerIndicator, GetPlatformDeviceName(), id)
  SetNameForPlayerIndicator(GetPlatformDeviceName(), id)    
end

ACTIVE_PLAYER_INDEX = { -1, -1, -1, -1 }
ACTIVE_PLAYER_COUNT = 0

function MPUpdateActivePlayers()
  local api = 0
  for i = 1, #PLAYERS do
    local c = PLAYERS[i]
    local visible = false
    if c ~= nil then
      if HasPlayerIndicatorName(i) then
        ACTIVE_PLAYER_INDEX[api] = i
        api = api + 1
        visible = true
      end
      c:SetVisible(visible)    
    end
  end
  ACTIVE_PLAYER_COUNT = api
end

function GetActivePlayerCount()
  return ACTIVE_PLAYER_COUNT
end

function GetActivePlayer(index)
  local id = ACTIVE_PLAYER_INDEX[index]
  if id == -1 then return nil end
  return PLAYERS[id]
end

function MPUpdatePlayerName()
  local player = GetPlayer()
  if player ~= nil then
    local id = player:GetId()
    Rpc(SetNameForPlayerIndicator, GetPlatformDeviceName(), id)
    SetNameForPlayerIndicator(GetPlatformDeviceName(), id)
  end
end

function UpdatePlayerList()
  -- we should NOT do this every frame!
  if IsMultiplayer() and GetGameStateFrames() % 63 == 0 then
    MPUpdatePlayerName()
    MPUpdateActivePlayers()
  end
end

function AcceptAs(id)
  print "*****AcceptAs*****"
  if not ACCEPTED then
    print ("Accepted as player ", id)    
    SetLocalPlayer(id, true)
    Multiplayer_ChangeId(0, id)
    DeferByTimeMs(500, "SetInGameState", id)
    ACCEPTED = true    
  end
end

function DeserializeAndAccept(id, data)
  print "*****DeserializeAndAccept*****"
  DeserializeGame(data)
  DeferByGameStateId(GAMESTATE_DESERIALIZED, "AcceptAs", id)   
end

function ConnectionData(id, data)
  if ALREADY_CONNECTED then
    -- it's meant for someone else
    return
  end
  print ("*****ConnectionData id=", id, "*****")
  RpcInitialize() -- reset 'SetThisUserId'  
--SetGameStateId(GAMESTATE_JOINING)
  DeferByGameStateId(GAMESTATE_INGAME, "DeserializeAndAccept", id, data)
  ALREADY_CONNECTED = true
end

function Server_OnConnect(id)
  print ("******** (Server) Connected to", id, "! ********")  
  SetPaused(false)
  local data = SerializeGame(GetPlayer())
  table.insert(CLIENTS, id)
  Rpc(ConnectionData, #CLIENTS, data)
  SetIndicatorVisibility("Player1Indicator", true)
  SetIndicatorVisibility("Player" .. id .. "Indicator", true)  
  SetNameForPlayerIndicator(GetPlatformDeviceName(), 1)
end

function Client_OnConnect(id)
  print ("******** (Client) Connected to", id, "! ********")
  SetPaused(false)
  table.insert(CLIENTS, id)
  SetIndicatorVisibility("Player1Indicator", true)
  SetIndicatorVisibility("Player" .. id .. "Indicator", true)  
end

function Server_OnDisconnect(id, reason)
  print ("******** (Server) Disconnected from", id, "for reason", reason, "! ********")
end

function Client_OnDisconnect(id, reason)
  print ("******** (Client) Disconnected from", id, "for reason", reason, "! ********")
end

function print_with_c(c, ...)
  local str = 'Calling function on charid=' .. tostring(c:GetId()) .. '; and remaining args ('
  for i,v in ipairs(arg) do
    str = str .. tostring(v) .. ','
  end
  print(str .. ')')
end

------------------------------------

function Apply(c, functionName, ...)
  local fn = _G[functionName]
  fn(c, unpack(arg))
end

function Call_ById(cId, functionName, ...)
  if cId == nil then return end
  local c = GetCharacterById(cId)
  Apply(c, functionName, unpack(arg))
end

function Call_MultiplayerNoLocal(c, functionName, ...)
  if IsMultiplayer() then
    local id = c:GetId()
    Rpc(Call_ById, id, functionName, unpack(arg))
  end
end

function Call_Multiplayer(c, functionName, ...)
  Apply(c, functionName, unpack(arg))
  Call_MultiplayerNoLocal(c, functionName, unpack(arg))
end

function MultiplayerDestroyCharacter(c)
  Call_Multiplayer(c, "DestroyCharacter")
end
