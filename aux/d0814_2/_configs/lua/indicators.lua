local HUD_BUTTON_SCALE  = 1.5
local HUD_BUTTON_ZSCALE = 0.7
local HUD_BUTTON_TEXT_SCALE = 1.0
local HUD_INDICATOR_SCALE  = 1.5
local HUD_INDICATOR_TEXT_SCALE = 1.5

function SetNameForPlayerIndicator(name, id)
  local indicatorName = "Player" .. id .. "Indicator"
  if name == nil then name = '' end
  local instance = GetIndicatorControl(indicatorName)
  UiControl_SetText(instance, name)
  if name == '' then  
    SetIndicatorVisibility(indicatorName, false)
    HideIndicator(indicatorName)
  else
    SetIndicatorVisibility(indicatorName, true)
    UiControl_SetText(instance, name)  
    ShowIndicator(indicatorName)    
  end
end

function InitializeIndicators()
  CreateIndicator("Player1Indicator")
  CreateIndicator("Player2Indicator")
  CreateIndicator("Player3Indicator")
  CreateIndicator("Player4Indicator")
  SetIndicatorVisibility("Player1Indicator", false)
  SetIndicatorVisibility("Player2Indicator", false)
  SetIndicatorVisibility("Player3Indicator", false)
  SetIndicatorVisibility("Player4Indicator", false)
  CreateIndicator("GoldIndicator")
  CreateIndicator("RedKeyIndicator")
  CreateIndicator("BlueKeyIndicator")
  CreateIndicator("WhiteKeyIndicator")
  CreateIndicator("IcePortalButton")
  CreateIndicator("FirePortalButton")
  CreateIndicator("SaveButton")
  CreateIndicator("TurnIndicator")
  CreateIndicator("HealPotionIndicator")
  ShowIndicators()
end

-- TIPS -------------

TIP_DURATION_FRAMES = 90
TIP_INSTANCES = {}
TIP_ASSOCIATED_INSTANCES = {}
TIP_DURATIONS = {}

function CreateTip(key, text, associatedInstance, x, y)
  local height = 15
  local height_2 = height * 0.5
  local instance = Ui_CreateButton(key .. "_tip", "dropshadow", text, x, y - height_2, x + 85, y + height_2, 1.0, 4.0 * HUD_SCALE * HUD_BUTTON_TEXT_SCALE)
  UiControl_AddChild(CANVAS, instance)  
  UiControl_SetVisible(instance, false)
  UiControl_SetActive(instance, false)
  TIP_INSTANCES[key] = instance
  TIP_ASSOCIATED_INSTANCES[key] = associatedInstance
  return instance  
end

function ShowTip(key)
  TIP_DURATIONS[key] = TIP_DURATION_FRAMES
end

function UpdateTip(key)
  local duration = TIP_DURATIONS[key]
  local instance = TIP_INSTANCES[key]
  if duration == nil or instance == nil then
    return
  end
  local visible = false  
  if duration > 0 then
    visible = true
    TIP_DURATIONS[key] = duration - 1
  else
    visible = false  
  end
  local associated = TIP_ASSOCIATED_INSTANCES[key]
  if associated ~= nil then
    visible = (visible and UiControl_IsVisible(associated))
  end
  _SetVisible(instance, visible)
end

function UpdateTips()
  for key, val in pairs(TIP_DURATIONS) do
    UpdateTip(key)  
  end
end

-- INDICATORS -------------

INDICATORS = {}

function MakeIndicator(instance, dynamicUpdate)
  local name = UiControl_GetName(instance)
  INDICATORS[name] = { instance=instance, name=name, text="", dynamicUpdate=dynamicUpdate, visibilityOverride=true }
  UiControl_AddChild(CANVAS, instance)  
  UiControl_SetVisible(instance, false)
--UiControl_SetActive(instance, false)
  UiControl_SetText(instance, INDICATORS[name].text)
end

function GetIndicatorControl(name)
  local item = INDICATORS[name]
  if item == nil then return nil end
  return item.instance
end

function SetIndicatorVisibility(name, visibility)
  local item = INDICATORS[name]
  if item == nil then return end
  item.visibilityOverride = visibility
end

function CreateIndicator(name)
  local instance = nil
  local dynamicUpdate = true
  
  ------------------------------------
  -- switch through and make the indicator ui instances --
  local offset = 75
  if     name == "Player1Indicator"  then instance = Ui_CreateImageButtonJustified     (name, "barb-icon64", "", 0, 0, -30 + offset, 25,  -5 + offset, 0.5, 3.0 * HUD_SCALE * HUD_INDICATOR_TEXT_SCALE, 0, 1, 1, 0, 0)
  elseif name == "Player2Indicator"  then instance = Ui_CreateImageButtonJustified     (name, "barb-icon64", "", 0, 0,  -5 + offset, 25,  20 + offset, 0.5, 3.0 * HUD_SCALE * HUD_INDICATOR_TEXT_SCALE, 0, 1, 1, 0, 0)    
  elseif name == "Player3Indicator"  then instance = Ui_CreateImageButtonJustified     (name, "barb-icon64", "", 0, 0,  20 + offset, 25,  45 + offset, 0.5, 3.0 * HUD_SCALE * HUD_INDICATOR_TEXT_SCALE, 0, 1, 1, 0, 0)  
  elseif name == "Player4Indicator"  then instance = Ui_CreateImageButtonJustified     (name, "barb-icon64", "", 0, 0,  45 + offset, 25,  70 + offset, 0.5, 3.0 * HUD_SCALE * HUD_INDICATOR_TEXT_SCALE, 0, 1, 1, 0, 0)
  elseif name == "GoldIndicator"     then instance = Ui_CreateRenderableButtonJustified(name, "Gold",        "", 0, 5,  60 + offset, 20, 110 + offset, 0.5, 4.0 * HUD_SCALE * HUD_INDICATOR_TEXT_SCALE, 0.5 * HUD_SCALE * HUD_INDICATOR_SCALE * 0.5, 3.6 * HUD_BUTTON_ZSCALE); --dynamicUpdate = false
  elseif name == "RedKeyIndicator"   then instance = Ui_CreateRenderableButtonJustified(name, "KeyRed0",     "", 0, 5,  90 + offset, 20, 140 + offset, 0.5, 4.0 * HUD_SCALE * HUD_INDICATOR_TEXT_SCALE, 0.5 * HUD_SCALE * HUD_INDICATOR_SCALE * 0.5, 3.6 * HUD_BUTTON_ZSCALE)
  elseif name == "BlueKeyIndicator"  then instance = Ui_CreateRenderableButtonJustified(name, "KeyBlue0",    "", 0, 5, 120 + offset, 20, 170 + offset, 0.5, 4.0 * HUD_SCALE * HUD_INDICATOR_TEXT_SCALE, 0.5 * HUD_SCALE * HUD_INDICATOR_SCALE * 0.5, 3.6 * HUD_BUTTON_ZSCALE)
  elseif name == "WhiteKeyIndicator" then instance = Ui_CreateRenderableButtonJustified(name, "KeyWhite0",   "", 0, 5, 150 + offset, 20, 200 + offset, 0.5, 4.0 * HUD_SCALE * HUD_INDICATOR_TEXT_SCALE, 0.5 * HUD_SCALE * HUD_INDICATOR_SCALE * 0.5, 3.6 * HUD_BUTTON_ZSCALE)
  elseif name == "TurnIndicator"     then instance = Ui_CreateRenderableButtonJustified(name, "Clock",       "", 2, 400, -5, 430, 35, 0.5, 7.0 * HUD_SCALE * HUD_BUTTON_TEXT_SCALE, 0.10 * HUD_SCALE * HUD_BUTTON_SCALE, 3.6 * HUD_BUTTON_ZSCALE)
  elseif name == "HealPotionIndicator" then instance = Ui_CreateRenderableButtonJustified(name, "Potion0",       "", 2, 512, 190, 512, 190, 1.0, 8.0 * HUD_SCALE * HUD_BUTTON_TEXT_SCALE, 1.0 * HUD_SCALE * HUD_BUTTON_SCALE, 3.6 * HUD_BUTTON_ZSCALE)
  elseif name == "IcePortalButton"   then
    instance = Ui_CreateRenderableButtonJustified(name, "Potion1", "", 2, 430, 30, 480, 70, 0.5, 6.0 * HUD_SCALE * HUD_BUTTON_TEXT_SCALE, 0.5 * HUD_SCALE * HUD_BUTTON_SCALE, 3.6 * HUD_BUTTON_ZSCALE)
    RegisterUiListener(name, "InventoryListener", GetInventoryListenerDataFor("Potion1"))
    CreateTip(name, "Ice Portal", instance, 380, 70)
  elseif name == "FirePortalButton"  then
    instance = Ui_CreateRenderableButtonJustified(name, "Potion5", "", 2, 430, 70, 480, 100, 0.5, 6.0 * HUD_SCALE * HUD_BUTTON_TEXT_SCALE, 0.5 * HUD_SCALE * HUD_BUTTON_SCALE, 3.6 * HUD_BUTTON_ZSCALE)
    RegisterUiListener(name, "InventoryListener", GetInventoryListenerDataFor("Potion5"))
    CreateTip(name, "Fire Portal", instance, 380, 100)
  elseif name == "SaveButton"        then
    instance = Ui_CreateRenderableButton(name, "Save", "", 430, -5, 480, 35, 0.5, 8.0 * HUD_SCALE * HUD_BUTTON_TEXT_SCALE, 0.10 * HUD_SCALE * HUD_BUTTON_SCALE, 3.6 * HUD_BUTTON_ZSCALE)
    RegisterUiListener(name, "SaveHudListener", "")
  end

  ------------------------------------
  
  if instance ~= nil then MakeIndicator(instance, dynamicUpdate) end
  return instance
end

function UpdateIndicator(name)
  -- $TODO --
end

function ShowIndicator(name)
  if INDICATORS[name] == nil then
    CreateIndicator(name)
  end
  local instance = INDICATORS[name].instance
  _SetVisible(instance, INDICATORS[name].visibilityOverride)
end

function SetIndicatorCount(name, prefix, suffix, count)
  SetIndicatorText(name, prefix, suffix, tostring(count))
  if count == nil or count <= 0 then
    HideIndicator(name)
  else
    ShowIndicator(name)
  end
end

function HideIndicator(name)
  if INDICATORS[name] == nil then
    CreateIndicator(name)
  end
  local instance = INDICATORS[name].instance
  _SetVisible(instance, false)
end

function UpdateIndicators()
  for key, val in pairs(INDICATORS) do
    UpdateIndicator(key)
  end
end

function ShowIndicators()
  for key, val in pairs(INDICATORS) do
    ShowIndicator(key)
  end
end

function HideIndicators()
  for key, val in pairs(INDICATORS) do
    HideIndicator(key)
  end
end

function SetIndicatorText(name, prefix, suffix, text)
  if INDICATORS[name] == nil then
    CreateIndicator(name)
  end
  local indicator = INDICATORS[name]
  if indicator == nil then
    return
  end
  local textstr = prefix .. tostring(text) .. suffix
  if indicator.text ~= text then
    UiControl_SetText(indicator.instance, textstr)
    ShowTip(name)    
    indicator.text = text
    if indicator.dynamicUpdate then
      UiControl_SetVisible(indicator.instance, false)
    end
    UiControl_SetVisible(indicator.instance, true)
  end  
end
