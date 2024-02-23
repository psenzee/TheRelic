print "Lua:animation.lua"

function ClearAction(c)
  c:SetAction("")
end

function ClearPlayerAction()
  ClearAction(GetPlayer())
end

function DisableMovement(c)
  c:SetCanMove(false)
end

function EnableMovement(c)
  c:SetCanMove(true)
end

function CanMove(c)
  return c:CanMove()
end

function ResolveCollision(c, p)
  local xa, ya, za = c:GetPosition()
  local xb, yb, zb = p:GetPosition()
  local d = v3Distance(xa, ya, 0, xb, yb, 0)
  if d >= characterRadius * 2.0 then
    return
  end
  local incr = 5.0
  local xi = (math.random() - 0.5) * incr
  local yi = (math.random() - 0.5) * incr  
  local lastd = d
  while d < characterRadius * 2.0 do 
    if d <= lastd then
      xa = xa + xi
      ya = ya + yi
    end     
    lastd = d
    xi = (math.random() - 0.5) * incr
    yi = (math.random() - 0.5) * incr
    d = v3Distance(xa + xi, ya + yi, 0, xb, yb, 0)
  end
  c:SetPosition(xa, ya, za)
end

function Update_InputDirection(c)
  local xi, yi, zi = GetInputDirection()
  local f = 1.0 / FPS_FACTOR
  c:SetForce(xi * f, yi * f, zi)
end
