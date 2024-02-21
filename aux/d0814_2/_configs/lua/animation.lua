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

function Update_InputDirection(c)
  local xi, yi, zi = GetInputDirection()
  c:SetForce(xi, yi, zi)
end
