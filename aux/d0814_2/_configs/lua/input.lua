print "Lua:input.lua"

LOCK_INPUT       = false
DISABLE_UI_INPUT = false
PROCESS_BUTTONS  = false

function _UnlockInput()
  ClearButtonPresses()
  LOCK_INPUT = false
end

function LockInputForTimeMs(ms)
  LOCK_INPUT = true
  DeferByTimeMs(ms, "_UnlockInput")
end

BUTTON_AREAS =
{
  {   0, 380,  58, 479 }, -- button 0
  { 128, 424, 196, 479 }, -- button 1
  {  53, 424, 112, 479 }, -- button 2
--{ 260, 424, 300, 479 }, -- button 3
  {   0, 0, 1, 1 }, -- button 3
  { 100, 180, 280, 300 }, -- button 4
}

THUMBSTICK = { 75, 85 }

THUMBSTICK_RADIUS = 50

THUMBSTICK_MOVEMENTS = {}

function SetThumbStickMovement(index, x, y)
  local tsx = _G.THUMBSTICK[1]
  local tsy = _G.THUMBSTICK[2]
  local tsr = _G.THUMBSTICK_RADIUS
  if v3Distance(x, y, 0, tsx, tsy, 0) < tsr then
    local indir = { -(y - tsy) / tsr, (x - tsx) / tsr, 0 }
    SetInputDirection(indir[1], indir[2], 0);
	_G.THUMBSTICK_MOVEMENTS[tostring(index)] = indir
    return true
    end
  return false
end

function UpdateThumbStickMovement(index, x, y)
  local str = tostring(index)
  if _G.THUMBSTICK_MOVEMENTS[str] ~= nil then
    local tsx = _G.THUMBSTICK[1]
    local tsy = _G.THUMBSTICK[2]
    local tsr = _G.THUMBSTICK_RADIUS  
    local indir = { -(y - tsy) / tsr, (x - tsx) / tsr, 0 }
    SetInputDirection(indir[1], indir[2], 0);
	_G.THUMBSTICK_MOVEMENTS[str] = indir
    return true
  end
  return false
end

function ClearThumbStickMovement(index)
  _G.THUMBSTICK_MOVEMENTS[tostring(index)] = nil
end

function ProcessInput()
  for k, v in pairs(_G.THUMBSTICK_MOVEMENTS) do
    SetInputDirection(v[1], v[2], 0);
  end
end

touchBeganPoint = { 0, 0 }
LOW_LEVEL_TOUCHED = false
TOUCHED = false

function TouchBegan(index, x, y)
  LOW_LEVEL_TOUCHED = true
  if LOCK_INPUT then
    TOUCHED = false
    return
  end
-- print("TouchBegan: ", x, ", ", y)
  if not DISABLE_UI_INPUT then
    Ui_NotifyInput("CLICK_BEGAN", x, y)
  end
  TOUCHED = true
  if PROCESS_BUTTONS == true and GetGameStateId() == GAMESTATE_INGAME then
    touchBeganPoint[1] = x
    touchBeganPoint[2] = y
    if not SetThumbStickMovement(index, x, y) then
      for i, box in ipairs(BUTTON_AREAS) do
        if x >= box[1] and x <= box[3] and
	       y >= box[2] and y <= box[4] then
           SetButtonPressed(i - 1)
        end
      end 
    end
  end
end

function TouchEnded(index, x, y)
  if LOCK_INPUT then
    return
  end
  if not DISABLE_UI_INPUT then  
    Ui_NotifyInput("CLICK_ENDED", x, y)
  end
  ClearThumbStickMovement(index)  
end 

function TouchMoved(index, x, y)
  if LOCK_INPUT then
    return
  end
  if not DISABLE_UI_INPUT then    
    Ui_NotifyInput("CLICK_MOVED", x, y)
  end
  UpdateThumbStickMovement(index, x, y)
end 

function TouchStationary(index, x, y)
  if LOCK_INPUT then
    return
  end
  if not DISABLE_UI_INPUT then
    Ui_NotifyInput("CLICK_STATIONARY", x, y)
  end
  UpdateThumbStickMovement(index, x, y)
end