print "Lua:hud.lua"

-- public functions
---
-- StartHud()
-- DrawHud(player)
-- IsMagicEnabled(player) [bool]
-- SetSuppressHud(bool)
-- SetTimerDuration(seconds)
-- SetTimerName(name)
-- AddTimerDuration(seconds)

HUD_SCALE = 3.75
TIMER_NAME = ""

function StartHud()
  RegisterListener("TimerEvent", "SecondTimerListener", {})
  SetTimerEvent("TimerEvent", 1000)
  InitializeIndicators()  
end

function DrawHud(player)
  if player ~= nil then
    if POTION_COUNT_TEXT == nil then
      SetPotionCountText("")
      ShowPotionCountText()
    end
    UpdateHud(player)
    if HUD_ON then
      DrawCombatButtons(player, -52 * HUD_SCALE, -30 * HUD_SCALE)
      DrawStickButton(player, 42 * HUD_SCALE, -22 * HUD_SCALE)
      DrawPauseButton(player, -56 * HUD_SCALE, 38 * HUD_SCALE)
      DrawPlaceNameAndHealth()
      DrawStats()
    end
  end
  if IsInLoadingState() then
    DrawLoading()
  end
end

function IsHudOn()
  return HUD_ON
end

function IsMagicEnabled(player)
  if player ~= nil then
    local data = player:data()
    return data.isMagicEnabled    
  end
  return false
end

function SetSuppressHud(value)
  SUPPRESS_HUD = value
  if value then
    HideTimerText()    
    HidePotionCountText()
  else
    ShowPotionCountText()
    if HUD_TIMER > 0 then
      ShowTimerText()
    end
  end
end

function SetTimerName(name)
  TIMER_NAME = name
end

function SetTimerDuration(seconds)
  HUD_TIMER = seconds
  if seconds > 0 then
  --  SetTimerText(tostring(HUD_TIMER))
    SetTimerText("")
    ShowTimerText()
  else
    HideTimerText()
  end  
end

function AddTimerDuration(seconds)
  if HUD_TIMER > 0 then
    SetTimerDuration(HUD_TIMER + seconds)
  else
    SetTimerDuration(seconds)
  end
end

-- PRIVATE ----------------------------------------------------------------------------------

HUD_ALPHA              = 1.0
BASE_HUD_ALPHA         = 0.5
IS_LOADING             = false
HUD_ON                 = true
LOADING_MESSAGE        = "Loading"
SUPPRESS_HUD           = false
HUD_TIMER              = -1
TIMER_TEXT             = nil
LEVEL_UP_BUTTON        = nil
POTION_COUNT_TEXT      = nil
PLAYER_ID_TEXT         = nil
PLAYER_ID_TEXT_TEXT    = nil
POTION_COUNT_TEXT_TEXT = nil
SHOW_HEAL_BUTTON       = false

function CreateTimerText()
  TIMER_TEXT = Ui_CreateButton("TimerText", "", "", 400, 10, 420, 30, 1.0, 22.5)
  UiControl_AddChild(CANVAS, TIMER_TEXT)
  UiControl_SetVisible(TIMER_TEXT, false)  
end

local HUD_BUTTON_SCALE  = 1.5
local HUD_BUTTON_ZSCALE = 0.7
local HUD_BUTTON_TEXT_SCALE = 1.0
local HUD_INDICATOR_SCALE  = 1.5
local HUD_INDICATOR_TEXT_SCALE = 1.5

function LevelUpButtonListener(info, data)
  if info == 'SELECTED' then
    --UiControl_SetVisible(LEVEL_UP_BUTTON, false)
    local player = GetPlayer()
    --player:SetLevel(player:GetLevelFromExperience())
    ToggleMap(ShowInGameAttributesPage)
    SetLevelUpMode(true)
  end
end

function CreateLevelUp()
  LEVEL_UP_BUTTON = Ui_CreateImageButton("LevelUpButton", "plus1_small", "", 440, 10, 470, 40, 1.0, 18.75, 0, 1, 1, 0, 0)
  UiControl_AddChild(CANVAS, LEVEL_UP_BUTTON)
  UiControl_SetVisible(LEVEL_UP_BUTTON, false)
  RegisterUiListener("LevelUpButton", "LevelUpButtonListener", {})
end

function CreatePotionCountText()
  POTION_COUNT_TEXT = Ui_CreateButton("PotionCountText", "", "", 460, 180, 470, 200, 1.0, 30)
  UiControl_AddChild(CANVAS, POTION_COUNT_TEXT)
  UiControl_SetVisible(POTION_COUNT_TEXT, false)  
end

function ShowTimerText()
  if TIMER_TEXT == nil then
    CreateTimerText()
  end
  if not UiControl_IsVisible(TIMER_TEXT) then
    UiControl_SetVisible(TIMER_TEXT, true)
  end
end

function ShowPotionCountText()
  if POTION_COUNT_TEXT == nil then
    CreatePotionCountText()
  end
  --[[
  if not UiControl_IsVisible(POTION_COUNT_TEXT) then
    UiControl_SetVisible(POTION_COUNT_TEXT, true)
  end
  ]]--
  UiControl_SetVisible(POTION_COUNT_TEXT, false)
end

function HidePotionCountText()
  _SetVisible(POTION_COUNT_TEXT, false)
end

function SetTimerText(text)
--[[
  if TIMER_TEXT == nil then
    CreateTimerText()
  end
  UiControl_SetText(TIMER_TEXT, TIMER_NAME .. " " .. text)
  if UiControl_IsVisible(TIMER_TEXT) then
    UiControl_SetVisible(TIMER_TEXT, false)
    UiControl_SetVisible(TIMER_TEXT, true) -- force a cool update look
  end
  ]]--
end

function SetPotionCountText(text)
  if POTION_COUNT_TEXT == nil then
    CreatePotionCountText()
  end
--[[  
  if text ~= POTION_COUNT_TEXT_TEXT then
    UiControl_SetText(POTION_COUNT_TEXT, text)
    if UiControl_IsVisible(POTION_COUNT_TEXT) then
      UiControl_SetVisible(POTION_COUNT_TEXT, false)
      UiControl_SetVisible(POTION_COUNT_TEXT, true) -- force a cool update look
    end
  end
  ]]--  
  POTION_COUNT_TEXT_TEXT = text
end

function HideTimerText()
  if TIMER_TEXT ~= nil then
    TIMER_NAME = ""
    UiControl_SetVisible(TIMER_TEXT, false)
  end
end

function HidePotionCountText()
  if POTION_COUNT_TEXT ~= nil then
    UiControl_SetVisible(POTION_COUNT_TEXT, false)
  end
end

function DrawAttackButton(player, x, y)
  if _buttons[0] then
    DrawQuadAt("blueringsword_hilite", HUD_ALPHA, x + (8 * HUD_SCALE), y - (4 * HUD_SCALE), 16 * HUD_SCALE, 16 * HUD_SCALE)
  else
    DrawQuadAt("blueringsword", HUD_ALPHA, x + (8 * HUD_SCALE), y - (4 * HUD_SCALE), 16 * HUD_SCALE, 16 * HUD_SCALE)
  end
end

function DrawMagicAttackButton(player, x, y)
  x = x - 4 * HUD_SCALE
  y = y + 12* HUD_SCALE
  if not IsMagicEnabled(player) then
    return
  end
  if _buttons[2] then
    DrawQuadAt("magic2_opaq_hilite", HUD_ALPHA, x, y, 14 * HUD_SCALE, 14 * HUD_SCALE)
  else
    DrawQuadAt("magic2_opaq", HUD_ALPHA, x, y, 14 * HUD_SCALE, 14 * HUD_SCALE)
  end
end

function DrawHealButton(player, x, y)
  if SHOW_HEAL_BUTTON == false then
    return
  end
  x = x - 4 * HUD_SCALE
  y = y + 28 * HUD_SCALE
  if _buttons[1] then
    DrawQuadAt("blueringfirepotion_opaq_hilite", HUD_ALPHA, x, y, 14 * HUD_SCALE, 14 * HUD_SCALE)
  else
    DrawQuadAt("blueringfirepotion_opaq", HUD_ALPHA, x, y, 14 * HUD_SCALE, 14 * HUD_SCALE)
  end
end

function DrawPauseButton(player, x, y)
  --x = x - 12
  --DrawQuadAt("pause2", 1.0, x, y, 8 * HUD_SCALE, 8 * HUD_SCALE)
end

function DrawStickButton(player, x, y)
  local size = 16 * HUD_SCALE
  local ringSize = size * 2
  DrawQuadAt("bluering", 0.5 * HUD_ALPHA, x, y, ringSize, ringSize)
  local xi, yi, zi = GetInputDirection()
  local mag = math.sqrt(xi * xi + yi * yi)
  local intensity = mag / 4.0
  local maxRadius = 6.0
  if mag > maxRadius then
    xi = xi / mag * maxRadius
    yi = yi / mag * maxRadius
  end
  DrawQuadAt("stick2",        (1.0 - intensity) * HUD_ALPHA, x + (xi * 2.5 * HUD_SCALE), y + (yi * 2.5 * HUD_SCALE), size, size)
  DrawQuadAt("stick2_hilite", (intensity) * HUD_ALPHA, x + (xi * 2.5 * HUD_SCALE), y + (yi * 2.5 * HUD_SCALE), size, size)
end

function DrawCombatButtons(player, x, y)
  DrawAttackButton(player, x, y)
  DrawMagicAttackButton(player, x, y)  
  DrawHealButton(player, x, y)
end

function DrawPlaceNameAndHealth()
  local player = GetPlayer()
  if player ~= nil then
    --Text_DrawString("[" .. GetLevelNumber() .. "]  " .. GetPlaceName(player:GetPosition()), (60 + 3) * HUD_SCALE, (37 + 2) * HUD_SCALE, 22.5, 0.5)
    Text_DrawString("[" .. GetLevelNumber() .. "]  ", (60 + 3) * HUD_SCALE, (37 + 2) * HUD_SCALE, 22.5, 0.5)

  -- Draw Health Bar

  local healthx = 25
  local healthy = 5
  local healthw = 200
  local healthh = 17

local hp = player:GetHitPoints()
local hpmax = player:GetMaxHitPoints()

if hpmax > 0 then

Ui_DrawQuad("health_bar_7", HUD_ALPHA * 0.75,
              healthx, healthy, 0.0,
              healthw, healthh, 0.0,  
              0, 1, 
              1, 0)  

local hpuv = (hp - hpmax * 0.01) / hpmax
Ui_DrawQuad("health", HUD_ALPHA * 0.75,
              healthx + 2, healthy + 2, 0.0,
              healthw - 2, healthh - 2, 0.0,
              0, hpuv, 
              1, hpuv)                             
              --[[
Ui_DrawQuad("BlueSelectorOpaque", HUD_ALPHA * 0.4,
              healthx, healthy, 0.0,
              healthw, healthh, 0.0,  
              0, 1, 
              1, 0)                ]]--
end

  end
end

function DrawLevelUp(player)
  if player ~= nil then
    --[[
    if LEVEL_UP_BUTTON == nil then
      CreateLevelUp()
    end
    if IsHudOn() and player:GetLevelFromExperience() > player:GetLevel() then
      UiControl_SetVisible(LEVEL_UP_BUTTON, true)
    else
      UiControl_SetVisible(LEVEL_UP_BUTTON, false)
    end
    ]]--
    --[[
    local leveled = false
    while player:GetLevelFromExperience() > player:GetLevel() do
      local nlvl = player:GetLevelFromExperience()
      player:SetHitPoints(player:GetMaxHitPoints(), player)
      player:SetMaxHitPoints(player:GetMaxHitPoints() * 1.1)
      player:SetStrength(player:GetStrength() + 1)
      local nlvlmod = nlvl % 2
      if nlvlmod == 0 then
        player:SetDexterity(player:GetDexterity() + 1)
      else
        player:SetVitality(player:GetVitality() + 1)      
        player:SetEnergy(player:GetEnergy() + 1)              
      end
      player:SetLevel(player:GetLevel() + 1)
      leveled = true
    end
    if leveled then
      MessageRise("\\#40ff40Level Up!\\#ffffff ", 0)    
    end
    ]]--    
  end
end

function DrawStats()
  local player = GetPlayer()
  if player == nil then
    return
  end
  local data = GetPlayer():data()
  local gold = 0
  if data.inventory ~= nil and data.inventory.Gold ~= nil then
    gold = data.inventory.Gold
  end
  Text_DrawString("kills " .. data.kills, (60 + 3) * HUD_SCALE, (32 + 2) * HUD_SCALE, 22.5, 0.5)
end

function UpdateHud(player)
  HUD_ON = not (IS_LOADING or SUPPRESS_HUD or GetGameStateId() ~= GAMESTATE_INGAME)
  if HUD_ON then
    local count = GetInventoryItemCount(GetPlayer(), 'Potion0')
    if count == 0 then
      SetPotionCountText("")
      SHOW_HEAL_BUTTON = false
    else
      SetPotionCountText(tostring(count))
      SHOW_HEAL_BUTTON = true
    end
    ShowIndicators()

    SetIndicatorText ('SaveButton',        "       ", "", " ")
    SetIndicatorText ('Player1Indicator', "      ", "", " ") --GetPlatformDeviceName())
    SetIndicatorText ('Player2Indicator', "      ", "", " ") --GetPlatformDeviceName())
    SetIndicatorText ('Player3Indicator', "      ", "", " ") --GetPlatformDeviceName())
    SetIndicatorText ('Player4Indicator', "      ", "", " ") -- GetPlatformDeviceName())
    SetIndicatorCount('GoldIndicator',     "    ", "", GetInventoryItemCount(GetPlayer(), 'Gold'))
    SetIndicatorCount('RedKeyIndicator',   "    ", "", GetInventoryItemCount(GetPlayer(), 'KeyRed'))
    SetIndicatorCount('BlueKeyIndicator',  "    ", "", GetInventoryItemCount(GetPlayer(), 'KeyBlue'))
    SetIndicatorCount('WhiteKeyIndicator', "    ", "", GetInventoryItemCount(GetPlayer(), 'KeyWhite'))
    SetIndicatorCount('IcePortalButton',   "", "    ", GetInventoryItemCount(GetPlayer(), 'Potion1'))
    SetIndicatorCount('FirePortalButton',  "", "    ", GetInventoryItemCount(GetPlayer(), 'Potion5'))
    SetIndicatorCount('HealPotionIndicator', "", "      ", GetInventoryItemCount(GetPlayer(), 'Potion0'))    
    SetIndicatorCount('TurnIndicator', "", "   ", HUD_TIMER)

  else
    HideIndicators()
  end
  UpdateTips()  
end

function RestoreLoadingMessage()
  LOADING_MESSAGE = "Loading"
end

function SetLoadingMessage(s)
  LOADING_MESSAGE = s
end

function SecondTimerListener(info, data)
  UpdateTimer()
  SetTimerEvent("TimerEvent", 1000)
end

function UpdateTimer()
  if HUD_TIMER > 0 then
    HUD_TIMER = HUD_TIMER - 1
    if HUD_TIMER == 0 then
      FireEvent("HudTimerExpired", "HudTimerExpired")
      HideTimerText()    
    else
      SetTimerText(tostring(HUD_TIMER))
    end
  end
end