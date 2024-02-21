function SerializeItem(item)
  local t = type(item)
  if t == "table" then
    return SerializeTable(item)
  elseif t == "string" then
    return "'" .. item .. "'"
  elseif t == "function" then
    -- don't put functions in
    return "$fn$"
  end
  return tostring(item)
end

function SerializeTable(table)
  local data = "{"
  for k, v in pairs(table) do
    local tv = type(v)
    data = data .. k .. "=" .. SerializeItem(v) .. ","
  end
  return data .. "}"
end

--local stuff = { hello="there", you=12.3, i="like candy", he={ "isweird", "likes strange things", 234.342 } }
--local stuff = { "xHI", "yTHERE" }
--local stuff = { x={ "isweird", "likes strange things", 234.342 } }
--local stuff = { hello="there", you=12.3, i="like candy", he={ "isweird", "likes strange things", 234.342 } }
local stuff = { hitPoints=1230, hitPointsMax=3843, experience=123443, attackCount=32, animationName="Attack" }

print("before: " .. SerializeItem(stuff))

local serialized = Serialize(stuff)
print("serialized size=" .. #serialized .. " bytes")
local deserialized = Deserialize(serialized)

print("after: " .. SerializeItem(deserialized))

Wait()