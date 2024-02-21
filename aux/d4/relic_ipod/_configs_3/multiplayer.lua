print "Lua:multiplayer.lua"

function HostGame()
    print("*********SERVER*********")
    Multiplayer_StartServer("Paul")
end

function JoinGame()
    print("*********CLIENT*********")
    Multiplayer_StartClient("Dylan")
    Multiplayer_Join("192.168.1.100")
end

PLAYERS = {}

function SetLocalPlayer(playerId, visible)
  local player = PLAYERS[playerId]
  SetPlayer(player)
  player:SetVisible(visible)
  for i = 1, #PLAYERS do
    PLAYERS[i]:data().isLocalPlayer = false
  end  
  player:data().isLocalPlayer = true
  RpcInitialize()
end

function CreatePlayers(count)
  for i = 1, count do
    local player = CreateNewPlayer(i)
    table.insert(PLAYERS, player)
    player:SetVisible(false)
  end
  SetLocalPlayer(1, false)
end

function StartMultiplayer(server)
  if server then
    HostGame()
  else
    JoinGame()
  end
end

function UpdateMultiplayer()
  if Multiplayer_IsJoined() and Multiplayer_GetId() ~= -1 then
    SetLocalPlayer(Multiplayer_GetId() + 1, true)
  else
    SetLocalPlayer(1, true)
  end
  RpcProcess()  
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
  SetThisUserId(GetPlayer():GetId())
end

function RpcProcess()
  while Sync_HasMessage() do
    HandleRpc(Sync_PopMessage())
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
  Sync_SendMessage(message)
  --Sync_SendMessage(PackRpc(toUserId, method, arg))
end

function Rpc(method, ...)
  RpcUser(0, method, unpack(arg))
  --Sync_SendMessage(PackRpc(0, method, arg))
end

-- PRIVATE IMPLEMENTATION

function HandleRpc(message)
  local success, fromUserId, method, arguments = ReadRpc(message)
  if success then
    method(unpack(arguments))
  end
  return success
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
  RpcUser(to, ReceiveCharacters, SerializeCharacterList())
end

function SerializeCharacterList()
  print ">>> serializing characters.."
  local chars = GetAllCharacters()
  local list = {}
  for i, v in ipairs(chars) do  
    if v:GetId() > 16 then
      table.insert(list, v:data())
    end
  end
  return list
end    

function DeserializeCharacterList(chars)
  for i, v in ipairs(chars) do  
    DeserializeCharacter(v)
  end
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