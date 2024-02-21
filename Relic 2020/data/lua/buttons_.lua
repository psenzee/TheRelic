print "Lua:buttons.lua"

THUMBSTICK_OVERALL_RADIUS = 0.5

if not GetPlatformIsiPad() then
--if true then
  BUTTON_AREAS =
  {
    {   0, 380,  58, 479 }, -- button 0
    { 118, 424, 186, 479 }, -- button 1
    {  53, 424, 112, 479 }, -- button 2
    {   0,   0,   1,   1 }, -- button 3
    { 100, 180, 280, 300 }, -- button 4
    {   0,   0,   1,   1 }, -- button 5
  }
  
  THUMBSTICK = { 75, 85 }
  THUMBSTICK_RADIUS = 50 * THUMBSTICK_OVERALL_RADIUS
  THUMBSTICK_DRAW_RADIUS = 150 
  HUD_BUTTON_SIZE = 50

else

  BUTTON_AREAS =
  {
    { -20, 380,  38, 479 }, -- button 0
    {  88, 424, 156, 479 }, -- button 1
    {  33, 424,  92, 479 }, -- button 2
    {   0,   0,   1,   1 }, -- button 3
    {  80, 180, 300, 300 }, -- button 4
    {   0,   0,   1,   1 }, -- button 5
  }
  
  THUMBSTICK = { 55, 85 }
  THUMBSTICK_RADIUS = 35 * THUMBSTICK_OVERALL_RADIUS
  THUMBSTICK_DRAW_RADIUS = 150
  HUD_BUTTON_SIZE = 50

end

function DrawTmp(x, y, ex, ey)
  local image = "white"
  Ui_DrawQuad(image, HUD_ALPHA,
              x, y, 0.0, ex, ey, 0.0,  
              0, 1, 1, 0)    
end

function DrawDebugTouchLayout()
  DrawTmp(THUMBSTICK[2] - THUMBSTICK_RADIUS, 320 - (THUMBSTICK[1] - THUMBSTICK_RADIUS), THUMBSTICK[2] + THUMBSTICK_RADIUS, 320 - (THUMBSTICK[1] + THUMBSTICK_RADIUS))
  for i, value in ipairs(BUTTON_AREAS) do
    DrawTmp(value[2], 320 - value[1], value[4], 320 - value[3])
  end
end

function DrawHudButton(index, image, hilite)
  if IsLocalButtonDown(index) then image = hilite end
  local button = BUTTON_AREAS[index + 1]
  local x  = button[2];  local y  = 320 - button[1]
  local ex = button[4];  local ey = 320 - button[3]
  Ui_DrawQuad(image, HUD_ALPHA, x, y, 0.0, ex, ey, 0.0, 0, 1, 1, 0)      
end

function DrawHudButton2(index, image, hilite)
  if IsLocalButtonDown(index) then image = hilite end
  local button = BUTTON_AREAS[index + 1]
  local x  = button[2];  local y  = 320 - button[1]
  local ex = button[4];  local ey = 320 - button[3]
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

function _DrawAttackButton()
  DrawHudButton2(0, "blueringsword", "blueringsword_hilite")
end

function _DrawMagicAttackButton()
  if not IsMagicEnabled(GetPlayer()) then return; end
  DrawHudButton2(2, "magic2_opaq", "magic2_opaq_hilite")
end

function _DrawHealButton()
  if SHOW_HEAL_BUTTON == false then return; end
  DrawHudButton2(1, "blueringfirepotion_opaq", "blueringfirepotion_opaq_hilite")  
end

function _DrawStickButton()
  local size = 60 -- $guess, gonna have to eyeball it
  local x = THUMBSTICK[2]
  local y = 320 - THUMBSTICK[1]
  DrawHudButtonCenteredAt("bluering", 0.5 * HUD_ALPHA, x, y, THUMBSTICK_DRAW_RADIUS, THUMBSTICK_DRAW_RADIUS)
  local xi, yi, zi = GetInputDirection()
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
  DrawHudButtonCenteredAt("stick2",        a1, offX, offY, size, size)
  DrawHudButtonCenteredAt("stick2_hilite", a2, offX, offY, size, size)
end

function DrawHudButtons()
  _DrawAttackButton()
  _DrawMagicAttackButton()
  _DrawHealButton()
  _DrawStickButton()
end