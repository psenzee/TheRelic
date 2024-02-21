print "Lua:buttons.lua"

DEBUG_SHIELD_TOUCH = nil

function GetDebugShieldTouch()
  if DEBUG_SHIELD_TOUCH == nil then
    DEBUG_SHIELD_TOUCH = Ui_CreateImageButton("debug-shield", "shield1", "", coors[1], coors[2], coors[3], coors[4], 1.0, 18.75, 0, 1, 1, 0, 0)
    UiControl_SetVisible(DEBUG_SHIELD_TOUCH, true)
  end
  return DEBUG_SHIELD_TOUCH
end

function DrawTmp(x, y, ex, ey)
  local image = "white"
  local thick = 2
  --Ui_DrawQuad(image, HUD_ALPHA, x, y, 0.0, ex, ey, 0.0, 0, 1, 1, 0)
  Ui_DrawQuad(image, HUD_ALPHA, x, y, 0.0, x + thick, ey, 0.0, 0, 1, 1, 0) -- left
  Ui_DrawQuad(image, HUD_ALPHA, ex - thick, y, 0.0, ex, ey, 0.0, 0, 1, 1, 0) -- right
  Ui_DrawQuad(image, HUD_ALPHA, x, y, 0.0, ex, y + thick, 0.0, 0, 1, 1, 0) -- top
  Ui_DrawQuad(image, HUD_ALPHA, x, ey - thick, 0.0, ex, ey, 0.0, 0, 1, 1, 0) -- bottom
end

function DrawDebugTouchLayout()
  local x0, y0, x1, y1 = GetReferenceBounds()
  DrawTmp(x0 + 5, y0 + 5, x1 - 5, y1 - 5)
  DrawTmp(THUMBSTICK[1] - THUMBSTICK_RADIUS, THUMBSTICK[2] - THUMBSTICK_RADIUS,
          THUMBSTICK[1] + THUMBSTICK_RADIUS, THUMBSTICK[2] + THUMBSTICK_RADIUS)
  for i, value in ipairs(BUTTON_AREAS) do
    DrawTmp(value[1], value[2], value[3], value[4])
  end
end

function DrawHudButton(index, image, hilite)
  if IsLocalButtonDown(index) then image = hilite end
  local button = BUTTON_AREAS[index + 1]
  local x  = button[1];  local y  = button[2]
  local ex = button[3];  local ey = button[4]
  Ui_DrawQuad(image, HUD_ALPHA, x, y, 0.0, ex, ey, 0.0, 0, 1, 1, 0)      
end

function DrawHudButton2(index, image, hilite)
  if IsLocalButtonDown(index) then image = hilite end
  local button = BUTTON_AREAS[index + 1]
  local x  = button[1];  local y  = button[2]
  local ex = button[3];  local ey = button[4]
  local dx = ex - x
  local dy = ey - y
  local cx = x + dx * 0.5
  local cy = y + dy * 0.5
  --Ui_DrawQuad(image, HUD_ALPHA, x, cy + dy * 0.5, 0.0, x - dy, cy - dy * 0.5, 0.0, 0, 1, 1, 0)      
  Ui_DrawQuad(image, HUD_ALPHA, x, cy - HUD_BUTTON_SIZE * 0.5, 0.0, x + HUD_BUTTON_SIZE, cy + HUD_BUTTON_SIZE * 0.5, 0.0, 0, 1, 1, 0)
end

function DrawHudButtonCenteredAt(image, alpha, x, y, w, h)
  w = w * 0.5; h = h * 0.5
  Ui_DrawQuad(image, alpha, x - w, y - h, 0.0, x + w, y + h, 0.0, 0, 1, 1, 0)
end

function DrawHudRingAt(image, alpha, x, y, w, h, angle)
  w = w * 0.5; h = h * 0.5
  Ui_DrawRing(image, alpha, x, y, 0.0, 50, 70, angle);
end

function _DrawAttackButton()
  DrawHudButton2(0, "black_button_sword", "blueringsword_hilite")
end

function _DrawMagicAttackButton()
  if not IsMagicEnabled(GetPlayer()) then return; end
  DrawHudButton2(2, "black_button_ring", "magic2_opaq_hilite")
end

function _DrawHealButton()
  if SHOW_HEAL_BUTTON == false then return; end
  DrawHudButton2(1, "black_button_potion", "blueringfirepotion_opaq_hilite")
end

THUMBSTICK_ANGLE = 0

function _DrawStickButton()
  local size = 60 -- $guess, gonna have to eyeball it
  local x = THUMBSTICK[1]
  local y = THUMBSTICK[2]
  local xi, yi, zi = GetRawInputDirection()
  local mag = math.sqrt(xi * xi + yi * yi)
  local intensity = mag / 4.0
  local maxRadius = --6.0
  --8.0
  12.0
  if mag > maxRadius then
    xi = xi / mag * maxRadius
    yi = yi / mag * maxRadius
  end
  local a1 = (1.0 - intensity) * HUD_ALPHA
  local a2 = (intensity) * HUD_ALPHA
  local STICK_SCALE = 5.0 -- $guess, gonna have to eyeball it
  local offX = x - (xi * STICK_SCALE)
  local offY = y - (yi * STICK_SCALE)
  local angle = GetAngle(0.0, 1.0, 0.0, xi, yi, zi)
  if angle ~= 0.0 and mag > 1.0 then
    THUMBSTICK_ANGLE = angle
  end
  DrawHudRingAt("tick_new_2", 0.5, x, y, size, size, THUMBSTICK_ANGLE)
end

function DrawDebugTouchSpot()
  -- primary spot
  local ox, oy, w, h = GetBackgroundOffsetSize()
  local ax, ay = GetTransformedTouchPoint()
  local sz = 10
  local ofx = 0
  local ofy = 0
  print(string.format(" xf %.2f %.2f\n", ax, ay))
  local x   = ax - sz + ofx
  local y   = ay - sz + ofy
  local ex  = ax + sz + ofx
  local ey  = ay + sz + ofy
  Ui_DrawQuad("stick2_hilite", 1, x, y, 0.0, ex, ey, 0.0, 0, 1, 1, 0)

  debug = GetDebugShieldTouch()
  UiControl_RequestBounds(debug, x, y, x + 30, y + 30)
end

function DrawHudButtons()
  _DrawAttackButton()
  _DrawMagicAttackButton()
  _DrawHealButton()
  _DrawStickButton()
  -- DrawDebugTouchLayout()
  --DrawDebugTouchSpot()
end
