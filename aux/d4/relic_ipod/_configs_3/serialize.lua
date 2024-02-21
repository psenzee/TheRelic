print "Lua:serialize.lua"

function NewCharacter(id, name, type, x, y, z)
  local c = Character.new(id, name, type, x, y, z)
  local data = c:data()
  data.constructor = { name = name, type = type, x = x, y = y, z = z, id = c:GetId() }
  return c
end

function SetCharacterId(character, id)
   character:SetId(id)
   character:data().constructor.id = id
end

function Copy(table, data)
  for k, v in pairs(table) do
    if (k ~= 'data') then
      data[k] = v
    end
  end
end

function SerializeTable(table)
  local data = "{"
  for k, v in pairs(table) do
    local tv = type(v)
    if tv == "table" then
      data = data .. k .. "="
      data = data .. SerializeTable(v) .. ","
    elseif tv == "string" then
          data = data .. k .. "='" .. v .. "',"
    elseif tv == "function" then
      --data = data .. k .. "=" .. tostring(v) .. ","
      -- don't put functions in
    else
      data = data .. k .. "=" .. tostring(v) .. ","      
    end   
  end
  return data .. "}"
end

function PrintTable(table, indent)
  print (SerializeTable(table))
end

function DeserializeCharacter(table)
  local ctr = table.constructor
  local c = Character.new(ctr.id, ctr.name, ctr.type, ctr.x, ctr.y, ctr.z)
  local data = c:data()
  Copy(table, data)
  if data.UpdateCharacterFn ~= nil then
    (data.UpdateCharacterFn)(c)
  end
  return c
end

function SerializeCharacter(character)
  return character:data()
end

function UpdateCharacterData(id, data)
  local c = GetCharacterById(id)
  Copy(data, c:data())
end