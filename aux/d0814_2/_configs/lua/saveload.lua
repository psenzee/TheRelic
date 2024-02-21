print "Lua:saveload.lua"

function GetAttributeTable(c)
  return
  {
    level     = c:GetLevel(),
    maxhp     = c:GetMaxHitPoints(),
    hp        = c:GetHitPoints(),
    xp        = c:GetExperience(),
    strength  = c:GetStrength(),
    dexterity = c:GetDexterity(),
    vitality  = c:GetVitality(),
    energy    = c:GetEnergy(),
    luck      = c:GetLuck()
  }
end

function SetAttributeTable(attrs, c)
  c:SetLevel(attrs.level)
  c:SetMaxHitPoints(attrs.maxhp)
  c:SetHitPoints(attrs.hp, c)
  c:SetExperience(attrs.xp)  
  c:SetStrength(attrs.strength)
  c:SetDexterity(attrs.dexterity)
  c:SetVitality(attrs.vitality)
  c:SetEnergy(attrs.energy)
  c:SetLuck(attrs.luck)
end

function GetBasicPlayerData(c)
  local x, y, z = c:GetPosition()
  return
  {
    character      = c:data(),
    attributes     = GetAttributeTable(c),    
    position       = { x, y, z },
    orientation    = c:GetOrientation(),
    gameLevel      = GetLevelNumber(),
  }
end

function SetBasicPlayerData(data, c)
  local gameLevel = data.gameLevel -- we need to figure out what to do here since we need to know the level in order to load it, before we set the character stuff (because they will be destroyed, then created)
  c:SetPosition(data.position[1], data.position[2], data.position[3])
  c:ForceOrientation(data.orientation)
  SetAttributeTable(data.attributes, c)  
  local cdata = c:data()
  for k, v in pairs(cdata) do
    cdata[k] = nil
  end
  deepcopyinto(data.character, cdata)
end

function SaveGame(c, filename)
  filename = MakeReadWritePath(filename)
  local x, y, z = c:GetPosition()
  UpdateListFromDoors()  
  local portals = MakeSaveablePortalList()
  local save =
  {
    character      = c:data(),
    attributes     = GetAttributeTable(c),
    position       = { x, y, z },
    gameLevel      = GetLevelNumber(),
    doors          = MakeSaveableDoorList(DOOR_LIST),
    portals        = portals,
    localGameState = LOCAL_GAME_STATE,
    visited        = GetVisitedData(),
    characters     = SerializeCharacterList(),
  }
  print ("Saving character data to '" .. filename .. "'") 
  local data = Serialize(save)
  local f = io.open(filename, 'wb')
  f:write(Compress(data))
  io.close(f)
  MessageRise("\\#80ff80Game Saved\\#ffffff ", 0)
end

function SerializeGame(c)
  local x, y, z = c:GetPosition()
  UpdateListFromDoors()  
  local portals = MakeSaveablePortalList()
  local save =
  {
    character      = c:data(),
    attributes     = GetAttributeTable(c),
    position       = { x, y, z },
    gameLevel      = GetLevelNumber(),
    doors          = MakeSaveableDoorList(DOOR_LIST),
    portals        = portals,
    localGameState = LOCAL_GAME_STATE,
    visited        = GetVisitedData(),
    characters     = SerializeCharacterList(),
  }
  print ("Serializing game")
  local data = Serialize(save)
  return Compress(data)
end
