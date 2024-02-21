print "Lua:upgrades.lua"

LEVEL_COMPLETE_MENU               = nil
UPGRADE_MENU_INCREASE_MAX_HEALTH  = nil
UPGRADE_MENU_INCREASE_MAX_MANA    = nil
UPGRADE_MENU_MAGIC_SHIELD         = nil
UPGRADE_MENU_MAGIC_BOLT           = nil
UPGRADE_MENU_DIVINE_TOUCH         = nil
UPGRADE_MENU_IMMOLATE             = nil
UPGRADE_MENU_ATOMIC_TOUCH         = nil

UPGRADE_MENUS                     = {}
UPGRADE_MENU_UNSPENT_KILLS        = {}
UPGRADE_MENU_COST_KILLS           = {}
UPGRADE_MENU_PURCHASE_BUTTON      = {}
UPGRADE_MENU_COST_REQUIREMENT     = {}
UPGRADE_MENU_REQUIREMENT          = {}
UPGRADE_MENU_REQUIREMENT_INSTANCE = {}

PURCHASABLES = 
{
  { "IncreaseMaxHealth", "Increase Maximum Health", "Increase maximum health by 10%.", 100, "None" },
  { "IncreaseMaxMana",   "Increase Maximum Mana",   "Increase maximum magic energy by 10%.", 100, "Amaranth Ring" },
  { "DivineTouch",       "Divine Touch",            "Release a powerful wave of energy.\nConsumes mana and requires an attack multiplier of x2.", 200, "Amaranth Ring" },
  { "MagicShield",       "Magic Shield",            "Create a magic shield that consumes mana.", 300, "Amaranth Ring" },
  { "Immolate",          "Immolate",                "Consume nearby enemies in pillars of fire.", 300, "Amaranth Ring" },
  { "MagicBolt",         "Magic Bolt",              "Fire a magic projectile that consumes mana.", 500, "Amaranth Ring" },
  { "AtomicTouch",       "Atomic Touch",            "Release a devastating wave of energy\nusing an ancient energy source.\nConsumes significant mana and requires\nan attack multiplier of x4.", 1000, "Amaranth Ring" },
}

function HideUpgradeMenus()
  _SetVisible(LEVEL_COMPLETE_MENU, false)  
  _SetVisible(UPGRADE_MENU_INCREASE_MAX_HEALTH, false)
  _SetVisible(UPGRADE_MENU_INCREASE_MAX_MANA, false)
  _SetVisible(UPGRADE_MENU_MAGIC_SHIELD, false)
  _SetVisible(UPGRADE_MENU_MAGIC_BOLT, false)
  _SetVisible(UPGRADE_MENU_DIVINE_TOUCH, false)
  _SetVisible(UPGRADE_MENU_IMMOLATE, false)
  _SetVisible(UPGRADE_MENU_ATOMIC_TOUCH, false)
end

local function _HideAnd(fn)
  HideMenusRestoreHud()
  fn()
end

local function _HideAndReturn(fn)
  HideMenusRestoreHud()
  fn()
  ShowLevelCompleteMenu()
end

function IsUpgradeEvent(name)
  if     name == 'BackToLevelCompleteMenu'    then _HideAnd(ShowLevelCompleteMenu)
  elseif name == 'Upgrade_IncreaseMaxHealth'  then _HideAnd(ShowUpgradeMenu_IncreaseMaxHealth)
  elseif name == 'Upgrade_IncreaseMaxMana'    then _HideAnd(ShowUpgradeMenu_IncreaseMaxMana)
  elseif name == 'Upgrade_MagicShield'        then _HideAnd(ShowUpgradeMenu_MagicShield)
  elseif name == 'Upgrade_MagicBolt'          then _HideAnd(ShowUpgradeMenu_MagicBolt)
  elseif name == 'Upgrade_DivineTouch'        then _HideAnd(ShowUpgradeMenu_DivineTouch)
  elseif name == 'Upgrade_AtomicTouch'        then _HideAnd(ShowUpgradeMenu_AtomicTouch)
  elseif name == 'Upgrade_Immolate'           then _HideAnd(ShowUpgradeMenu_Immolate)
  
  elseif name == 'Purchase_IncreaseMaxHealth' then _HideAndReturn(Purchase_IncreaseMaxHealth)
  elseif name == 'Purchase_IncreaseMaxMana'   then _HideAndReturn(Purchase_IncreaseMaxMana)
  elseif name == 'Purchase_MagicShield'       then _HideAndReturn(Purchase_MagicShield)
  elseif name == 'Purchase_MagicBolt'         then _HideAndReturn(Purchase_MagicBolt)
  elseif name == 'Purchase_DivineTouch'       then _HideAndReturn(Purchase_DivineTouch)
  elseif name == 'Purchase_AtomicTouch'       then _HideAndReturn(Purchase_AtomicTouch)
  elseif name == 'Purchase_Immolate'          then _HideAndReturn(Purchase_Immolate)
  else return false
  return true
end

function AddUpgradeButton(parent, name, text, width)
  local button = Ui_CreateButton(name, "darkborder", text, 0, 0, width, 25, 1.0, 20)
  UiControl_SetMargin(button, 2, 2)
  UiControl_AddChild(parent, button)
  RegisterUiListener(name, "MenuListener", { name = name })
  return button
end

function CreateUpgradeMenus()
  CreateLevelCompleteMenu()
  CreateUpgradeMenu_IncreaseMaxHealth()
  CreateUpgradeMenu_IncreaseMaxMana()
  CreateUpgradeMenu_MagicShield()
  CreateUpgradeMenu_MagicBolt()
  CreateUpgradeMenu_DivineTouch()
  CreateUpgradeMenu_AtomicTouch() 
  CreateUpgradeMenu_Immolate()  
end

function GetKills()
  local player = GetPlayer()
  if player == nil then return 0 end
  local data = player:data()
  if data == nil then return 0 end  
  return data.kills
end

function GetUnspentKills()
  local player = GetPlayer()
  if player == nil then return 0 end
  local data = player:data()
  if data == nil then return 0 end  
  return data.unspentKills
end

function SetTextForUpgradeOption(instance, text, active)
  local color = "\\#c0c0c0"
  if active then color = "\\#80ff80" end
  UiControl_SetText(instance, color .. text)
end

function SatisfiesUpgradeCriteria(requiresMagic, unspentKillsCost, isMaxedOut)
  if isMaxedOut then
    return false
  end
  local player = GetPlayer()
  if player == nil then return false end
  local data = player:data()
  if data == nil then return false end
  if requiresMagic and not data.isMagicEnabled then
    return false
  end
  local unspentKills = GetUnspentKills()
  if unspentKillsCost > unspentKills then
    return false
  end
  return true
end

function UpdateLevelCompleteMenu()
  local player = GetPlayer()
  if player == nil then return end
  local data = player:data()
  if data == nil then return end
  UiControl_SetText(LEVEL_COMPLETE_MENU_UPGRADE_TEXT, "You have \\#80ff80" .. GetUnspentKills() .. "\\#ffffff unspent kill(s).")
  SetTextForUpgradeOption(LEVEL_COMPLETE_MENU_INC_MAX_HEALTH, "Increase Maximum Health", SatisfiesUpgradeCriteria(false, 100, false))
  SetTextForUpgradeOption(LEVEL_COMPLETE_MENU_INC_MAX_MANA,   "Increase Maximum Mana", SatisfiesUpgradeCriteria(true, 100, false))
  SetTextForUpgradeOption(LEVEL_COMPLETE_MENU_MAGIC_BOLT,     "Magic Bolt", SatisfiesUpgradeCriteria(true, 500, data.isBoltEnabled))
  SetTextForUpgradeOption(LEVEL_COMPLETE_MENU_MAGIC_SHIELD,   "Magic Shield", SatisfiesUpgradeCriteria(true, 300, data.isShieldEnabled)) 
  SetTextForUpgradeOption(LEVEL_COMPLETE_MENU_DIVINE_TOUCH,   "Divine Touch", SatisfiesUpgradeCriteria(true, 200, data.isDivineTouchEnabled))
  SetTextForUpgradeOption(LEVEL_COMPLETE_MENU_IMMOLATE,       "Immolate", SatisfiesUpgradeCriteria(true, 300, data.isImmolateEnabled))
  SetTextForUpgradeOption(LEVEL_COMPLETE_MENU_ATOMIC_TOUCH,   "Atomic Touch", SatisfiesUpgradeCriteria(true, 1000, data.isAtomicTouchEnabled))  
end

function CreateLevelCompleteMenu()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  local top = 10 --MENUS_TOP - 25
  local height = 20
  local size = 23
  local dividerHeight = 15  
  LEVEL_COMPLETE_MENU = Ui_CreateMenu("LevelCompleteMenu", "border3", "..", 240 - halfWidth, top, 240 + halfWidth, top, 1.0, 22.5)
  AddTextLine(LEVEL_COMPLETE_MENU, "Level Complete!", height * 2.0, size * 1.5)
  LEVEL_COMPLETE_MENU_UPGRADE_TEXT = AddTextLine(LEVEL_COMPLETE_MENU, "..", height, size)
  AddTextLine(LEVEL_COMPLETE_MENU, "", dividerHeight, size)
  LEVEL_COMPLETE_MENU_INC_MAX_HEALTH = AddUpgradeButton(LEVEL_COMPLETE_MENU, "Upgrade_IncreaseMaxHealth",  "Increase Max Health",  width)
  LEVEL_COMPLETE_MENU_INC_MAX_MANA = AddUpgradeButton(LEVEL_COMPLETE_MENU, "Upgrade_IncreaseMaxMana",  "Increase Max Mana",  width)
  LEVEL_COMPLETE_MENU_DIVINE_TOUCH = AddUpgradeButton(LEVEL_COMPLETE_MENU, "Upgrade_DivineTouch",  "Divine Touch",  width)  
  LEVEL_COMPLETE_MENU_MAGIC_SHIELD = AddUpgradeButton(LEVEL_COMPLETE_MENU, "Upgrade_MagicShield",  "Magic Shield",  width)  
  LEVEL_COMPLETE_MENU_IMMOLATE = AddUpgradeButton(LEVEL_COMPLETE_MENU, "Upgrade_Immolate",  "Immolate",  width)  
  LEVEL_COMPLETE_MENU_MAGIC_BOLT = AddUpgradeButton(LEVEL_COMPLETE_MENU, "Upgrade_MagicBolt",  "Magic Bolt",  width) 
  LEVEL_COMPLETE_MENU_ATOMIC_TOUCH = AddUpgradeButton(LEVEL_COMPLETE_MENU, "Upgrade_AtomicTouch",  "Atomic Touch",  width)     
  AddTextLine(LEVEL_COMPLETE_MENU, "", dividerHeight, size)  
  AddMenuButton(LEVEL_COMPLETE_MENU, "LoadComplete_NextLevel", "Continue", width)  
  UiControl_LayoutVertical(LEVEL_COMPLETE_MENU)
  UiControl_AddChild(CANVAS, LEVEL_COMPLETE_MENU)
  UiControl_SetVisible(LEVEL_COMPLETE_MENU, false)
end

function AreRequirementsSatisfied(name)
  local req = UPGRADE_MENU_REQUIREMENT[name]
  if req == 'None' then
    return true
  elseif req == 'Amaranth Ring' then
    local player = GetPlayer()
    if player == nil then return false end
    local data = player:data()
    if data == nil then return false end  
    return data.isMagicEnabled
  end
  return false
end

function ShowUpgradeMenu(instance, name)
  local unspent = GetUnspentKills()
  UiControl_SetText(UPGRADE_MENU_UNSPENT_KILLS[name], "\\#808080You have \\#80ff80" .. unspent .. "\\#808080 unspent kill(s).")
  local costColor = "\\#80ff80"
  local requireColor = "\\#80ff80"
  local purchaseColor = "\\#ffffff"
  local purchaseButton = UPGRADE_MENU_PURCHASE_BUTTON[name]
  local cost = UPGRADE_MENU_COST_KILLS[name]
  local satisfied = AreRequirementsSatisfied(name)
  if cost >= unspent or not satisfied then
    purchaseColor = "\\#808080"
    UiControl_SetActive(purchaseButton, false)
    if cost >= unspent then costColor = "\\#ff0000" end
    if not satisfied then requireColor = "\\#ff0000" end    
  else
    UiControl_SetActive(purchaseButton, true)
  end
  UiControl_SetText(UPGRADE_MENU_PURCHASE_BUTTON[name], purchaseColor .. "Purchase")
  UiControl_SetText(UPGRADE_MENU_COST_REQUIREMENT[name], "\\#808080Cost: " .. costColor .. "" .. cost .. " kills")
  UiControl_SetText(UPGRADE_MENU_REQUIREMENT_INSTANCE[name], "\\#808080Requirements: " .. requireColor .. UPGRADE_MENU_REQUIREMENT[name])
  HideMenus()
  SetPaused(true)
  PROCESS_BUTTONS = false
  SetSuppressHud(true)
  SetDarkness(0.5)
  _SetVisible(instance, true)
end

function CreateUpgradeMenu_IncreaseMaxHealth()
  UPGRADE_MENU_INCREASE_MAX_HEALTH = CreateUpgradeMenu("IncreaseMaxHealth", "Increase Maximum Health", "Increase maximum health by 10%.", 100, "None")
end

function ShowUpgradeMenu_IncreaseMaxHealth()
  ShowUpgradeMenu(UPGRADE_MENU_INCREASE_MAX_HEALTH, "IncreaseMaxHealth")
end

function CreateUpgradeMenu_IncreaseMaxMana()
  UPGRADE_MENU_INCREASE_MAX_MANA = CreateUpgradeMenu("IncreaseMaxMana", "Increase Maximum Mana", "Increase maximum magic energy by 10%.", 100, "Amaranth Ring")
end

function ShowUpgradeMenu_IncreaseMaxMana()
  ShowUpgradeMenu(UPGRADE_MENU_INCREASE_MAX_MANA, "IncreaseMaxMana")
end

function CreateUpgradeMenu_MagicShield()
  UPGRADE_MENU_MAGIC_SHIELD = CreateUpgradeMenu("MagicShield", "Magic Shield", "Create a magic shield that consumes mana.", 300, "Amaranth Ring")
end

function ShowUpgradeMenu_MagicShield()
  ShowUpgradeMenu(UPGRADE_MENU_MAGIC_SHIELD, "MagicShield")
end

function CreateUpgradeMenu_MagicBolt()
  UPGRADE_MENU_MAGIC_BOLT = CreateUpgradeMenu("MagicBolt", "Magic Bolt", "Fire a magic projectile that consumes mana.", 500, "Amaranth Ring")
end

function ShowUpgradeMenu_MagicBolt()
  ShowUpgradeMenu(UPGRADE_MENU_MAGIC_BOLT, "MagicBolt")
end

function CreateUpgradeMenu_DivineTouch()
  UPGRADE_MENU_DIVINE_TOUCH = CreateUpgradeMenu("DivineTouch", "Divine Touch", "Release a powerful wave of energy.", 200, "Amaranth Ring")
end

function ShowUpgradeMenu_DivineTouch()
  ShowUpgradeMenu(UPGRADE_MENU_DIVINE_TOUCH, "DivineTouch")
end

function CreateUpgradeMenu_AtomicTouch() 
  UPGRADE_MENU_ATOMIC_TOUCH = CreateUpgradeMenu("AtomicTouch", "Atomic Touch", "Release a devastating wave of energy\nusing an ancient energy source.", 1000, "Amaranth Ring")
end

function ShowUpgradeMenu_AtomicTouch()
  ShowUpgradeMenu(UPGRADE_MENU_ATOMIC_TOUCH, "AtomicTouch")
end

function Purchase_IncreaseMaxHealth()
  local player = GetPlayer()
  if player == nil then return false end
  local hpmax = player:GetMaxHitPoints()
  player:SetMaxHitPoints(hpmax * 1.1)
  player:SetHitPoints(hpmax * 1.1)
  data.unspentKills = data.unspentKills - UPGRADE_MENU_COST_KILLS["IncreaseMaxHealth"]  
  return true  
end

function Purchase_IncreaseMaxMana()
  local player = GetPlayer()
  if player == nil then return false end
  local data = player:data()
  if data == nil then return false end
  if data.manaMax == nil then return false end
  data.manaMax = data.manaMax * 1.1
  data.mana = data.manaMax
  data.unspentKills = data.unspentKills - UPGRADE_MENU_COST_KILLS["IncreaseMaxMana"]  
  return true  
end

function Purchase_AtomicTouch()
  local player = GetPlayer()
  if player == nil then return false end
  local data = player:data()
  if data == nil then return false end  
  data.isAtomicTouchEnabled = true
  data.unspentKills = data.unspentKills - UPGRADE_MENU_COST_KILLS["AtomicTouch"]
  return true  
end

function Purchase_DivineTouch()
  local player = GetPlayer()
  if player == nil then return false end
  local data = player:data()
  if data == nil then return false end  
  data.isDivineTouchEnabled = true
  data.unspentKills = data.unspentKills - UPGRADE_MENU_COST_KILLS["DivineTouch"]
  return true  
end

function Purchase_Immolate()
  local player = GetPlayer()
  if player == nil then return false end
  local data = player:data()
  if data == nil then return false end  
  data.isImmolateEnabled = true
  data.unspentKills = data.unspentKills - UPGRADE_MENU_COST_KILLS["Immolate"]
  return true  
end

function Purchase_MagicShield()
  local player = GetPlayer()
  if player == nil then return false end
  local data = player:data()
  if data == nil then return false end  
  data.isShieldEnabled = true
  data.unspentKills = data.unspentKills - UPGRADE_MENU_COST_KILLS["MagicShield"]
  return true
end

function Purchase_MagicBolt()
  local player = GetPlayer()
  if player == nil then return false end
  local data = player:data()
  if data == nil then return false end  
  data.isBoltEnabled = true
  data.unspentKills = data.unspentKills - UPGRADE_MENU_COST_KILLS["MagicBolt"]
  return true
end

function CreateUpgradeMenu_Immolate()
  UPGRADE_MENU_IMMOLATE = CreateUpgradeMenu("Immolate", "Immolate", "Consume nearby enemies in pillars of fire.", 300, "Amaranth Ring")
end

function ShowUpgradeMenu_Immolate()
  ShowUpgradeMenu(UPGRADE_MENU_IMMOLATE, "Immolate")
end

function CreateUpgradeMenu(name, formattedName, text, cost, requirements)
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  local top = 10
  local height = 20
  local size = 23
  UPGRADE_MENU_COST_KILLS[name] = cost
  UPGRADE_MENU_REQUIREMENT[name] = requirements
  local dividerHeight = 15  
  local upgradeMenu = Ui_CreateMenu("UpgradeMenu_" .. name, "border3", "..", 240 - halfWidth, top, 240 + halfWidth, top, 1.0, 22.5)
  AddTextLine(upgradeMenu, formattedName, height * 2.0, size * 1.5)
  AddTextLine(upgradeMenu, "", dividerHeight, size)
  UPGRADE_MENU_UNSPENT_KILLS[name] = AddTextLine(upgradeMenu, "..", height, size)  
  AddTextLine(upgradeMenu, "", dividerHeight, size)
  AddTextLine(upgradeMenu, text, height, size) 
  AddTextLine(upgradeMenu, "", dividerHeight * 4, size)
  UPGRADE_MENU_COST_REQUIREMENT[name] = AddTextLine(upgradeMenu, "\\#808080Cost: \\#ffffff" .. cost .. " kills", height, size)
  UPGRADE_MENU_REQUIREMENT_INSTANCE[name] = AddTextLine(upgradeMenu, "\\#808080Requirements: \\#ffffff" .. requirements, height, size) 
  AddTextLine(upgradeMenu, "", dividerHeight, size)
  UPGRADE_MENU_PURCHASE_BUTTON[name] = AddMenuButton(upgradeMenu, "Purchase_" .. name, "Purchase", width)  
  AddMenuButton(upgradeMenu, "BackToLevelCompleteMenu", "Return", width)  
  UiControl_LayoutVertical(upgradeMenu)
  UiControl_AddChild(CANVAS, upgradeMenu)
  UiControl_SetVisible(upgradeMenu, false)
  return upgradeMenu
end
