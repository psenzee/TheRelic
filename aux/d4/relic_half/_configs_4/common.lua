print "Lua:common.lua"

PI     = 3.141592653589793
TWOPI  = PI * 2.0
HALFPI = PI * 0.5

function Random(variance)
  return math.random() * variance - (variance * 0.5)
end

function GetWeighted(array, value)
  local count = #array
  local weight = 0.0
  for i = 1, count do
    weight = weight + array[i].weight
    if value <= weight then
        return array[i]
    end
  end
  return array[count]
end

function Nothing2(a, b)
end
