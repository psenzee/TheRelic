print "Lua:globals.lua"

PI     = 3.141592653589793
TWOPI  = PI * 2.0
HALFPI = PI * 0.5

SIGNAL_NONE = 0
SIGNAL_DELETE = 1
    
SIGNAL_DIED = 2
SIGNAL_DEATH_COMPLETE = 3
SIGNAL_ABSORBED = 4
SIGNAL_RECEIVED_HIT = 5
SIGNAL_ATTACK_START = 6
SIGNAL_ATTACK_COMPLETE = 7
SIGNAL_LANDED_HIT = 8
SIGNAL_EVADED_HIT = 9

BASE_EXPERIENCE_MULTIPLIER = 2

CONSTRAINED_COUNT_ENEMY_TYPE = 1111

-- HUD ---------------

function SetHudGlobals(bounds)

  local button_w            = 60
  local button_h            = 80
  local button_column_w     = 75

  _G.THUMBSTICK_OFFSET      = 80
  _G.THUMBSTICK_RADIUS      = 25
  _G.THUMBSTICK_DRAW_RADIUS = 150
  _G.HUD_BUTTON_SIZE        = 50

  _G.UI_BOUNDS = bounds

  _G.UI_LEFT   = _G.UI_BOUNDS[1]
  _G.UI_TOP    = _G.UI_BOUNDS[2]
  _G.UI_RIGHT  = _G.UI_BOUNDS[3]
  _G.UI_BOTTOM = _G.UI_BOUNDS[4]
  _G.UI_CENTER = {
    (_G.UI_RIGHT - _G.UI_LEFT) * 0.5 + _G.UI_LEFT,
    (_G.UI_BOTTOM - _G.UI_TOP) * 0.5 + _G.UI_TOP
  }

  local inner_column_left  = _G.UI_RIGHT - button_column_w * 2.0
  local inner_column_right = inner_column_left + button_w
  local outer_column_left  = inner_column_left + button_column_w
  local outer_column_right = outer_column_left + button_w

  _G.BUTTON_ATTACK = { inner_column_left, 240, inner_column_right, 320 } -- button sword/attack
  _G.BUTTON_HEALTH = { outer_column_left,  20, outer_column_right, 100 } -- button health
  _G.BUTTON_SHIELD = { outer_column_left, 240, outer_column_right, 320 } -- button shield
  _G.BUTTON_MAGIC  = { outer_column_left, 130, outer_column_right, 210 } -- button magic/ring
  _G.BUTTON_PAUSE  = {                                                   -- button pause (on player character)
    _G.UI_CENTER[1] - button_w,
    _G.UI_CENTER[2] - button_h,
    _G.UI_CENTER[1] + button_w,
    _G.UI_CENTER[2] + button_h
  }

  _G.BUTTON_AREAS =
  {
    _G.BUTTON_ATTACK, -- button 0 sword
    _G.BUTTON_HEALTH, -- button 1 health
    _G.BUTTON_MAGIC,  -- button 2 ring
    _G.BUTTON_SHIELD, -- button 3 shield
    _G.BUTTON_PAUSE,  -- button 4 player
    { 0, 0, 1, 1 }, -- button 5
  }

  _G.THUMBSTICK = {
    _G.UI_LEFT + _G.THUMBSTICK_OFFSET,
    _G.UI_BOTTOM - _G.THUMBSTICK_OFFSET
  }
  -- _G.THUMBSTICK_ANGLE = 0
end

_G.THUMBSTICK_ANGLE = 0

SetHudGlobals({ 0, 0, 10, 10 })

function UpdateHudGlobals()
  local minx, miny, maxx, maxy = GetUiBounds()
  SetHudGlobals({ minx, miny, maxx, maxy })
end
