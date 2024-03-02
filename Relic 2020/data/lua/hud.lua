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
MIN_MAGIC_ATTACK_COST = 50

function StartHud()
  RegisterListener("TimerEvent", "SecondTimerListener", {})
  SetTimerEvent("TimerEvent", 1000)
  InitializeIndicators()  
end

function DrawAlignment()
  local x, y, w, h = GetUiBounds()
  local sz = 30
  local s2 = sz / 2
  Ui_DrawStringCentered("xy", x+s2, y+s2, sz, 1.0)
  Ui_DrawStringCentered("xh", x+s2, h-s2, sz, 1.0)
  Ui_DrawStringCentered("wy", w-s2, y+s2, sz, 1.0)
  Ui_DrawStringCentered("wh", w-s2, h-s2, sz, 1.0)
  Ui_DrawStringCentered("cc", w * 0.5 + x, h * 0.5 + y, sz, 1.0)
end

function DrawMapHud(player)
  if IsMapOn() then
    local x, y, w, h = GetUiBounds()
    local gold = GetInventoryItemCount(GetPlayer(), 'Gold')
    local player = GetPlayer()
    local kills = 0
    local souls = 0
    local data = nil
    if player ~= nil then
      data = player:data()
      if data ~= nil then
        souls = data.unspentKills
        kills = data.kills
        Ui_DrawStringCentered("L" .. tostring(GetLevelNumber()) .. "/" .. GetPlaceName(player:GetPosition()), w * 0.5 + x, h - 50, 20, 1.0)
        Ui_DrawStringCentered(string.format("%d gold  %d kills  %d unspent kills", gold, kills, souls), w * 0.5 + x, h - 25, 25, 1.0)
      end
    end
  end
end

function DrawHud(player)
  if player ~= nil then
    if POTION_COUNT_TEXT == nil then
      SetPotionCountText("")
      ShowPotionCountText()
    end
    UpdateHud(player)
    if HUD_ON then
	  DrawHudButtons()
	  DrawPlaceNameAndHealth()
      DrawStats()
    end
  end
  if IsInLoadingState() then
    DrawLoading()
  end
  DrawMapHud(player)
end

function IsHudOn()
  return HUD_ON
end

function IsMagicEnabled(player)
  if player ~= nil then
    local data = player:data()
    return data.isMagicEnabled and data.mana >= MIN_MAGIC_ATTACK_COST
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

HUD_SCALE_FACTOR       = 1.0
HUD_TOP_OFFSET         = 0.0

if GetPlatformIsiPad() then
  HUD_SCALE_FACTOR = 0.75
  HUD_TOP_OFFSET   = -20.0
end

function CreateTimerText()
  TIMER_TEXT = Ui_CreateButton("TimerText", "", "", 400, 10, 420, 30, 1.0, 22.5 * HUD_SCALE_FACTOR)
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
  -- TODO this apparently isn't used
  coors = BUTTON_HEALTH -- health
  POTION_COUNT_TEXT = Ui_CreateButton("PotionCountText", "", "", coors[1], coors[2], coors[3], coors[4], 1.0, 30)
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

function DrawBar(fillImage1, fillImage2, x, y, w, h, value)
  -- Draw Health/Mana/Multiplier/etc. Bar
  
  if value > 1.0 then value = 1.0 end
  if value < 0.0 then value = 0.0 end

  Ui_DrawQuad("health_bar_8", HUD_ALPHA * 0.75,
              x, y, 0.0, x + w, y + h, 0.0,  
              0, 1, 1, 0)  
  if fillImage1 == fillImage2 or fillImage2 == '' then
      Ui_DrawQuad(fillImage1, HUD_ALPHA * 0.75,
                  x, y + 2, 0.0, x + w * value, y + h - 2, 0.0,
                  0, value, 1, value)
  else
      Ui_DrawQuad(fillImage1, HUD_ALPHA * 0.75 * (1.0 - value),
                  x, y + 2, 0.0, x + w * value, y + h - 2, 0.0,
                  0, value, 1, value)  
      Ui_DrawQuad(fillImage2, HUD_ALPHA * 0.75 * value,
                  x, y + 2, 0.0, x + w * value, y + h - 2, 0.0,
                  0, value, 1, value)
  end
              
-- Ui_DrawQuad("health_border", HUD_ALPHA, x, y, 0.0, x + w, y + h, 0.0, 0, 1, 1, 0)
end

function DrawBarRatio(fillImage1, fillImage2, x, y, w, h, value, max)
  if max > 0 then -- draw only if denominator is not zero
    DrawBar(fillImage1, fillImage2, x, y, w, h, value / max)
  end
end

function DrawLevelInformation(player)
--[[
  local x, y, w, h = GetUiBounds()
  Ui_DrawString("L" .. tostring(GetLevelNumber()), x + 5, y + 20, 30, 1.0)
  Ui_DrawString(" " .. GetPlaceName(player:GetPosition()), x + 5, h - 15, 20, 0.6)
  Ui_DrawString(string.format("mv: %d", GetMovableCount()), w * 0.8, h - 15, 20, 0.6)
]]--
end

function DrawPlaceNameAndHealth()
  local player = GetPlayer()
  if player ~= nil then
    DrawLevelInformation(player)

    -- Draw Health Bar

    local healthx = 30
    local healthy = 5
    local manax = 30
    local manay = 14
    local hitbarx = 30
    local hitbary = 24
    local healthw = 170
    local healthh = 10
    local manaw = 170
    local manah = 10
    local hitbarw = 170
    local hitbarh = 6

    local data = player:data()
    local hpmax = player:GetMaxHitPoints()
    local hp = player:GetHitPoints()
    local hpw_factor = 0.06
    healthw = hpmax * hpw_factor
    local manamax = data.manaMax
    local mana = data.mana
    local manaw_factor = 0.6
    manaw = manamax * manaw_factor
  
    local maxw = healthw
    if manaw > healthw then maxw = manaw end
    DrawBarRatio("red2", "", healthx * HUD_SCALE_FACTOR, healthy * HUD_SCALE_FACTOR + HUD_TOP_OFFSET, healthw * HUD_SCALE_FACTOR, healthh * HUD_SCALE_FACTOR, hp, hpmax)
    if data.isMagicEnabled then
      DrawBarRatio("blue2", "", manax * HUD_SCALE_FACTOR, manay * HUD_SCALE_FACTOR + HUD_TOP_OFFSET, manaw * HUD_SCALE_FACTOR, manah * HUD_SCALE_FACTOR, mana, manamax)
    end
    if data.multiplier == nil then data.multiplier = 1.0 end
    DrawBarRatio("blue2", "green2", hitbarx * HUD_SCALE_FACTOR, hitbary * HUD_SCALE_FACTOR + HUD_TOP_OFFSET, maxw * HUD_SCALE_FACTOR, hitbarh * HUD_SCALE_FACTOR, data.multiplier - 1.0, 3.0)
  --DrawBarRatio("green2", "", 20, 310, 440, 5, GetLevelNumber() - 1, 12)
 
 -- Text_DrawString(string.format("\n\n%.02f", GetFpsAverage()), (60 + 3) * HUD_SCALE, (37 + 2) * HUD_SCALE, 30, 1.0)

  end
end

function ShieldListener(info, data)
  print("info = " .. info)
  if info == 'CLICK_LEAVE' or info == 'CLICK_ENDED' then
    SetShieldEnabled(GetPlayer(), false)
    print("disabling shield")
  elseif info == 'CLICK_BEGAN' or info == 'CLICK_ENTERED' or info == 'FOCUSED' then
    SetShieldEnabled(GetPlayer(), true)
    print("enabling shield")
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
--[[
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
  ]]--
end

function GetColorText(value, startr, startg, startb, endr, endg, endb)
  value = value - math.floor(value)
  return "\\#" .. HexColor(value * (endr - startr) + startr, value * (endg - startg) + startg, value * (endb - startb) + startb)
end

function GetColorTextAlpha(value, startr, startg, startb, starta, endr, endg, endb, enda)
  value = value - math.floor(value)
  return "\\%" .. HexColorAlpha(value * (endr - startr) + startr, value * (endg - startg) + startg, value * (endb - startb) + startb, value * (enda - starta) + starta)
end

local lastSpellType = ''
local multiplierAchievements = { false, false, false, false, false, false, false, false, false }

function UpdateHud(player)
  Lux_EnableLighting(LUX_PASS_UI, false)

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

    local kills = 0
    local unspentKills = 0
    local player = GetPlayer()
    local data = nil
    if player ~= nil then data = player:data() end
    if data ~= nil and data.kills ~= nil then kills = data.kills end
    if data ~= nil and data.unspentKills ~= nil then unspentKills = data.unspentKills end
    
    local gold = GetInventoryItemCount(GetPlayer(), 'Gold')
    SetAchievementValue('Gold', gold)

    SetIndicatorText ('SaveButton',        "       ", "", " ")
    SetIndicatorText ('Player1Indicator', "      ", "", " ") --GetPlatformDeviceName())
    SetIndicatorText ('Player2Indicator', "      ", "", " ") --GetPlatformDeviceName())
    SetIndicatorText ('Player3Indicator', "      ", "", " ") --GetPlatformDeviceName())
    SetIndicatorText ('Player4Indicator', "      ", "", " ") -- GetPlatformDeviceName())
    SetIndicatorCount('GoldIndicator',     "    ", "", gold)
    SetIndicatorText ('KillsIndicator',     "     ", "", "" .. kills .. "  (" .. unspentKills .. ")")
    SetIndicatorCountMinNoText('RedKeyIndicator',   "    ", "", GetInventoryItemCount(GetPlayer(), 'KeyRed'), 1)
    SetIndicatorCountMinNoText('BlueKeyIndicator',  "    ", "", GetInventoryItemCount(GetPlayer(), 'KeyBlue'), 1)
    SetIndicatorCountMinNoText('WhiteKeyIndicator', "    ", "", GetInventoryItemCount(GetPlayer(), 'KeyWhite'), 1)
    SetIndicatorCount('IcePortalButton',   "", "    ", GetInventoryItemCount(GetPlayer(), 'Potion1'))
    SetIndicatorCount('FirePortalButton',  "", "    ", GetInventoryItemCount(GetPlayer(), 'Potion5'))
    SetIndicatorCount('HealPotionIndicator', "", "      ", GetInventoryItemCount(GetPlayer(), 'Potion0'))

    HideIndicator('SaveButton')
    HideIndicator('GoldIndicator')
    HideIndicator('KillsIndicator')

    local data = GetPlayer():data()
    if data.hitcount == nil then data.hitcount = 0 end    
    if data.multiplier == nil then data.multiplier = 1.0 end
    local hits       = data.hitcount    
    local multiplier = data.multiplier
    
    local prefix = GetColorTextAlpha(GetPlayer():data().multiplier, 1.0, 1.0, 1.0, 0.2, 0.0, 1.0, 1.0, 1.0)
	local displayMultiplier = math.floor(GetPlayer():data().multiplier)
    --SetIndicatorCountMin('HitMultiplierIndicator', "\\#00ff00x", "      ", displayMultiplier, 2)
    SetIndicatorCountMin('HitMultiplierIndicator', prefix .. "x", "      ", displayMultiplier, 2)
	if not multiplierAchievements[displayMultiplier] then
	  multiplierAchievements[displayMultiplier] = true
	  ShowAchievement("x" .. displayMultiplier)
	end
    --SetIndicatorCountMin('HitCountIndicator', "", "      ", math.floor(hits), 2)
    SetIndicatorCountMin('HitCountIndicator', "", "      ", 0, 2)
    --SetIndicatorCount('TurnIndicator', "", "   ", HUD_TIMER) 
    SetIndicatorCount('TurnIndicator', "turn :", "   ", HUD_TIMER)
	
    local spellType = ""
    if not data.isMagicEnabled or data.mana < MIN_MAGIC_ATTACK_COST then
      spellType = ""
    else
      --if     multiplier >= 4.0 and data.mana >= MIN_MAGIC_ATTACK_COST * 4.0 then 
      if     multiplier >= 5.0 and data.mana >= MANA_ATOMIC_TOUCH and data.isAtomicTouchEnabled then
--        spellType = "cataclysm" -- "\\%40ff40ffcataclysm"
        spellType = GetColorTextAlpha((GetGameStateFrames() % 20) * 0.05, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0) .. "cataclysm"
        --"\\%ffffffffcataclysm"
      elseif multiplier >= 3.0 and data.mana >= MIN_MAGIC_ATTACK_COST * 3.0 and data.isImmolateEnabled then
        spellType = "immolate" -- "\\%40ff40ffimmolate"
      elseif multiplier >= 2.0 and data.mana >= MIN_MAGIC_ATTACK_COST * 2.0 and data.isDivineTouchEnabled then
        spellType = "divine touch" -- "\\%40ff40ffdivine touch"
      elseif data.mana >= MIN_MAGIC_ATTACK_COST then
        spellType = ""
      end    
    end
    if spellType == '' then
      if lastSpellType ~= '' then
        SetIndicatorText('SpellTypeIndicator', "", "", spellType)
        HideIndicator('SpellTypeIndicator')
      end
    elseif spellType ~= lastSpellType then
      SetIndicatorText('SpellTypeIndicator', "", "", spellType)
      --ShowIndicator('SpellTypeIndicator')
    end
    lastSpellType = spellType
    
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
