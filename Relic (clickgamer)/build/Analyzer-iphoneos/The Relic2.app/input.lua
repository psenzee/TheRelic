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

THUMBSTICK_MOVEMENTS = {}
 
THUMBSTICK_SIZE_SCALE         =   4.0
THUMBSTICK_INPUT_SCALE        =  12.0
THUMBSTICK_TIME               =   0.0
THUMBSTICK_TIME_DECAY         = 500.0
THUMBSTICK_TIME2              =   0.0

THUMBSTICK_LAST_INPUT         = { 0.0, 0.0, 0.0 }

function ResetThumbStickTime()
    THUMBSTICK_TIME = GetTime()
end

THUMBSTICK_IN_PROGRESS         = { false, false, false, false, false, false, false }
THUMBSTICK_FIELD_VECTOR_WEIGHT = 0.5
THUMBSTICK_RESCALE             = 8.0

function ThumbStick_SetInputDirection(x, y, z, rescale)
  local p = GetPlayer()
  if rescale then
    local xn, yn, zn = v3Normal(x, y, z)
    x = xn * THUMBSTICK_RESCALE; y = yn * THUMBSTICK_RESCALE; z = zn * THUMBSTICK_RESCALE
  end
  SetRawInputDirection(x, y, z)   
  if p == nil then
    SetInputDirection(x,
                      y, 
					  z) 
  else
    local xp, yp, zp = p:GetPosition()
    local xv, yv, zv = GetFieldVectorAt(xp + x, yp + y, zp + z)
    SetInputDirection(x - xv * THUMBSTICK_FIELD_VECTOR_WEIGHT, 
                      y - yv * THUMBSTICK_FIELD_VECTOR_WEIGHT, 
                      z - zv * THUMBSTICK_FIELD_VECTOR_WEIGHT)
  end
end

function AnyThumbStickInProgress()
    for i, v in ipairs(THUMBSTICK_IN_PROGRESS) do
	  if v ~= false then
	    return true
	  end
	end
	return false
end

function GetThumbStickTimeScale()
    if AnyThumbStickInProgress() then
	  ResetThumbStickTime()
	  return 1.0
	end
    local time = GetTime() - THUMBSTICK_TIME
	if (time > THUMBSTICK_TIME_DECAY) then 
	  return 0.0
	end	
    return 1.0 - (time / THUMBSTICK_TIME_DECAY)
end

function SetThumbStickInProgress(index, value)
  THUMBSTICK_IN_PROGRESS[index] = value
end

function SetThumbStickMovement(index, x, y, timescale)
  local tsx = _G.THUMBSTICK[1]
  local tsy = _G.THUMBSTICK[2]
  local tsr = _G.THUMBSTICK_RADIUS * THUMBSTICK_SIZE_SCALE
  if THUMBSTICK_IN_PROGRESS[index] or v3Distance(x, y, 0, tsx, tsy, 0) < tsr then
    local indir = { -(y - tsy) / tsr, (x - tsx) / tsr, 0 }
    ThumbStick_SetInputDirection(indir[1] * THUMBSTICK_INPUT_SCALE * timescale, indir[2] * THUMBSTICK_INPUT_SCALE * timescale, 0, true);
	_G.THUMBSTICK_MOVEMENTS[tostring(index)] = indir
    THUMBSTICK_TIME2 = 0	
	ResetThumbStickTime()
	SetThumbStickInProgress(index, true)
	THUMBSTICK_LAST_INPUT = nil
    return true
  end
--SetThumbStickInProgress(index, false)  
  return false
end

function ClearThumbStickMovement(index)
  _G.THUMBSTICK_MOVEMENTS[tostring(index)] = nil
end

function ProcessInput()
  if THUMBSTICK_TIME2 ~= 0 and not AnyThumbStickInProgress() then
	local time = GetTime() - THUMBSTICK_TIME2
	if time > THUMBSTICK_TIME_DECAY then
	  THUMBSTICK_TIME2 = 0
	else
	  local scale = (1.0 - (time / THUMBSTICK_TIME_DECAY))
	  if THUMBSTICK_LAST_INPUT ~= nil and THUMBSTICK_LAST_INPUT[1] ~= nil then
	    ThumbStick_SetInputDirection(THUMBSTICK_LAST_INPUT[1] * scale, THUMBSTICK_LAST_INPUT[2] * scale, THUMBSTICK_LAST_INPUT[3] * scale, false)
	  end
	end
  end
end

touchBeganPoint = { 0, 0 }
LOW_LEVEL_TOUCHED = false
TOUCHED = false

touchInProgress = { -1, -1, -1, -1, -1 }
hudDisableEventLine = 240

function TouchBegan(index, x, y)
  LOW_LEVEL_TOUCHED = true
  if LOCK_INPUT then
    TOUCHED = false
    return
  end
-- print("TouchBegan: ", x, ", ", y)
  if not DISABLE_UI_INPUT then
    if not IsHudOn() or y > hudDisableEventLine then -- $HACK if hud is enabled, then only send events from the right half of the screen, to make the shield button work properly
      Ui_NotifyInput("CLICK_BEGAN", index, x, y)
    end
  end
  SetThumbStickMovement(index, x, y, 1.0)  
  TOUCHED = true
  if PROCESS_BUTTONS == true and GetGameStateId() == GAMESTATE_INGAME then
    touchBeganPoint[1] = x
    touchBeganPoint[2] = y
    --if not SetThumbStickMovement(index, x, y, 1.0) then
      for i, box in ipairs(BUTTON_AREAS) do
        if x >= box[1] and x <= box[3] and
	       y >= box[2] and y <= box[4] then
           SetButtonPressed(i - 1)
--         touchInProgress[index] = i - 1
        end
      end 
    --end
  end
end

function TouchEnded(index, x, y)
  if LOCK_INPUT then
    return
  end
  if not DISABLE_UI_INPUT then  
    if not IsHudOn() or y > hudDisableEventLine then -- $HACK if hud is enabled, then only send events from the right half of the screen, to make the shield button work properly
      Ui_NotifyInput("CLICK_ENDED", index, x, y)
    end    
  end
  SetThumbStickInProgress(index, false)
 -- SetInputDirection(0.0, 0.0, 0.0)
  THUMBSTICK_TIME2 = GetTime()  
  local x, y, z = GetRawInputDirection()
  if x == nil then x = 0.0 end
  if y == nil then y = 0.0 end
  if z == nil then z = 0.0 end
  THUMBSTICK_LAST_INPUT = { x, y, z }
end 

function TouchMoved(index, x, y)
  if LOCK_INPUT then
    return
  end
  if not DISABLE_UI_INPUT then        
    if not IsHudOn() or y > hudDisableEventLine then -- $HACK if hud is enabled, then only send events from the right half of the screen, to make the shield button work properly
      Ui_NotifyInput("CLICK_MOVED", index, x, y)
    end        
  end
  SetThumbStickMovement(index, x, y, 1.0)
end 
