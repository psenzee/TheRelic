print "Lua:common.lua"

PI     = 3.141592653589793
TWOPI  = PI * 2.0
HALFPI = PI * 0.5

math.randomseed(0)

function Random(variance)
  return math.random() * variance - (variance * 0.5)
end

-- this (or some variant) should be in C++
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

function Nothing(a, b)
end
  
function GetInputDirection()
  local scale = 800.0  
  local xi, yi, zi = GetVectorInput()  
  return v3Clamp(xi * scale, yi * scale, zi * scale, -10.0, 10.0)
end