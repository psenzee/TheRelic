print "lua:inventory.lua"

function TurnEnemies15Spell()
  TurnEnemies(15)
end

function KillCircle1000Spell()
  local c = GetPlayer()
  DoFlash(500)
  HitCircleInRadius(c, 256.0, 50.0, 500.0)
  Call_Multiplayer(c, "SetAreaAttackEffect2")
end

function ImmolateCircleSpell()
  DoFlash(500)
  ImmolateCircleInRadius(c, 128.0, 50.0, 5.0)
end

function KillCircle100Spell()
  local c = GetPlayer()
  DoFlash(500)
  HitCircleInRadius(c, 256.0, 50.0, 100.0)
  Call_Multiplayer(c, "SetAreaAttackEffect3")
end

function TurnEnemies30Spell()
  TurnEnemies(30)
end

function LesserHealSpell()
  local player = GetPlayer()
  if player ~= nil then
    MultiplayerHeal(player, player:GetMaxHitPoints())
    SetHealEffect(player)
    local xp, yp, zp = player:GetPosition()
    Audio_PlayAt("Magic", xp, yp, zp, 1.0)
  end
end

function FullManaRestoreSpell()
  local player = GetPlayer()
  if player ~= nil then
    local data = player:data()
    data.mana = data.manaMax
    SetHealEffect(player)
    local xp, yp, zp = player:GetPosition()
    Audio_PlayAt("Magic", xp, yp, zp, 1.0)
  end
end

function PartialManaRestoreSpell()
  local player = GetPlayer()
  if player ~= nil then
    local data = player:data()
    data.mana = data.mana + data.manaMax * 0.2
    if data.mana > data.manaMax then
      data.mana = data.manaMax
    end
    --SetHealEffect(player)
    local xp, yp, zp = player:GetPosition()
    Audio_PlayAt("Magic", xp, yp, zp, 1.0)
  end
end

function fwd_ShowGoldTip()     ShowGoldTip()     end
function fwd_ShowHealthTip()   ShowHealthTip()   end
function fwd_ShowMagicTip()    ShowMagicTip()    end
function fwd_ShowTurnTip()     ShowTurnTip()     end
function fwd_ShowRedKeyTip()   ShowRedKeyTip()   end
function fwd_ShowBlueKeyTip()  ShowBlueKeyTip()  end
function fwd_ShowWhiteKeyTip() ShowWhiteKeyTip() end
function fwd_ShowManaTip()     ShowManaTip()     end

inventoryNames =
{
  KeyWhite  = { "a",  "White Key",               "s", "KeyWhite0", "\nx",                         "",                          nil,                  false, nil, nil, fwd_ShowWhiteKeyTip },
  KeyRed    = { "a",  "Red Key",                 "s", "KeyRed0",   "\nx",                         "",                          nil,                  false, nil, nil, fwd_ShowRedKeyTip },
  KeyBlue   = { "a",  "Blue Key",                "s", "KeyBlue0",  "\nx",                         "",                          nil,                  false, nil, nil, fwd_ShowBlueKeyTip },
  Gold      = { "a",  "Gold",                    "",  "Gold",      "\n",                          "\n1",                       nil,                  false, nil, nil, fwd_ShowGoldTip },
  Potion0   = { "a",  "Heal",                    "",  "Potion0",   "\nheal x",                    "\nheal",                    LesserHealSpell,      false, nil, nil, fwd_ShowHealthTip },
  Potion4   = { "a",  "Mana Restore",            "",  "Potion4",   "\nmana x",                    "\nmana",                    PartialManaRestoreSpell, true,  nil, nil, fwd_ShowManaTip },
  Potion2   = { "an", "Atomic Touch",            "s", "Potion2",   "\nx",                         "",                          KillCircle1000Spell,  true,  nil, nil, nil },  
  Potion3   = { "a",  "Divine Touch",            "s", "Potion3",   "\nx",                         "",                          KillCircle100Spell,   true,  nil, nil, nil },
--Potion4   = { "a",  "Genetic Touch",           "s", "Potion4",   "\nx",                         "",                          KillCircle100Spell,   true,  nil, nil, nil },
  Potion6   = { "a",  "Demon's Touch",           "s", "Potion6",   "\nx",                         "",                          KillCircle100Spell,   true,  nil, nil, nil },
  Potion7   = { "a",  "Immolate",                "s", "Potion7",   "immolate x",                  "immolate",                  ImmolateCircleSpell,  true,  nil, nil, nil },  
  Potion8   = { "a",  "Hell's Touch",            "s", "Potion8",   "\nx",                         "",                          KillCircle1000Spell,  true,  nil, nil, nil },
  Book4     = { "a",  "Turn Enemies Spell",      "",  "Book4",     "\nturn x",                    "\nturn",                    TurnEnemies30Spell,   true,  nil, nil, fwd_ShowTurnTip },
  Potion5   = { "a",  "Fire Portal",             "s", "Potion5",   "fire\nportal x",              "fire\nportal",              PlacePortalA,         false, nil, nil, nil },
  Book2     = { "a",  "Turn Enemies Spell",      "s", "Book2",     "\nx",                         "",                          TurnEnemies15Spell,   true,  nil, nil, fwd_ShowTurnTip },
  Book3     = { "a",  "Blue Spellbook",          "s", "Book3",     "\nx",                         "",                          KillCircle100Spell,   true,  nil, nil, nil },
  Potion1   = { "a",  "Ice Portal",              "s", "Potion1",   "ice\nportal x",               "ice\nportal",               PlacePortalB,         false, nil, nil, nil }, 
  RingQuest = { "an", "Amaranth Ring Quest",     "s", "BookQuest", "\namaranth ring quest x",     "\namaranth ring quest",     nil,                  false, nil, nil, nil },
  Gen1Quest = { "a",  "Destroy Generator Quest", "s", "BookQuest", "\ndestroy generator quest x", "\ndestroy generator quest", nil,                  false, nil, nil, nil },  
  Ring      = { "an", "Amaranth Ring",           "s", "Ring",      "\namaranth\nring x",          "\namaranth\nring",          nil,                  false, nil, nil, fwd_ShowMagicTip },
}

INVENTORY_ARTICLE         = 1
INVENTORY_NAME            = 2
INVENTORY_SUFFIX          = 3
INVENTORY_TYPE            = 4
INVENTORY_MULTIPLE_PREFIX = 5
INVENTORY_SINGLE_PREFIX   = 6
INVENTORY_FUNCTION        = 7
INVENTORY_IMMEDIATE       = 8
INVENTORY_UICONTROL       = 9
INVENTORY_LISTENER        = 10
INVENTORY_FIRST_FUNCTION  = 11

function ItemPhrase(item, count)
  local name = inventoryNames[item]
  if count == 0 then
    return "no " .. name[INVENTORY_NAME] .. name[INVENTORY_SUFFIX]
  elseif count == 1 then
    return name[INVENTORY_ARTICLE] .. " " .. name[INVENTORY_NAME]
  end
  return count .. " " .. name[INVENTORY_NAME] .. name[INVENTORY_SUFFIX]
end

INVENTORY_MENU        = nil
INVENTORY_CIRCLE_MENU = nil
INVENTORY_RADIUS      = 100.0

function InventoryListener(info, data)
  if info ~= "SELECTED" then
    return
  end
  print("++++++USED '" .. data.key .. " (" .. info .. ")'+++++++")
  if data.execute ~= nil then
    local found = RetrieveFromInventory(GetPlayer(), data.key, 1)
    if found then
      if IsMapOn() then
        ToggleMap() -- here assuming that map is on
      end
      data.execute()
    end
  end
end

function GetInventoryListenerDataFor(key)
  local data = inventoryNames[key]
  return { key = key, execute = data[INVENTORY_FUNCTION] }
end

function Invoke(key)
  InventoryListener("SELECTED", GetInventoryListenerDataFor(key))
end

function InvokeHealPotion()
  if GetPlayer():GetHitPoints() < GetPlayer():GetMaxHitPoints() * 0.80 then
    Invoke("Potion0")
  end
end

function InvokeIcePortal()
  Invoke("Potion1")
end

function InvokeFirePortal()
  Invoke("Potion5")
end

function UiInventoryCreate()
  INVENTORY_MENU = Ui_CreateMenu("Inventory", "border3", "..", 0, 0, 480, 320, 1.0, 6.0 * HUD_SCALE)
  INVENTORY_CIRCLE_MENU = Ui_CreateMenu("InvCircle", "border3", "..", 0, 0, 480, 320, 1.0, 6.0 * HUD_SCALE)
  for key,data in pairs(inventoryNames) do
    local type = data[INVENTORY_TYPE]
    local item = nil
    if key == "Gold" then
      item = Ui_CreateImageButton("Gold", "gold", "..", 0, 0, 25, 25, 1.0, 5.0 * HUD_SCALE, 0, 1, 1, 0, 0)
    else
      item = Ui_CreateRenderableButton(key, type, data[INVENTORY_NAME], 0, 0, 50, 50, 1.0, 4.0 * HUD_SCALE, 0.5 * HUD_SCALE, 3.6)
    end
    data[INVENTORY_LISTENER] = RegisterUiListener(key, "InventoryListener", { key = key, execute = data[INVENTORY_FUNCTION] })
    data[INVENTORY_UICONTROL] = item
    UiControl_SetVisible(item, false)
    UiControl_AddChild(INVENTORY_CIRCLE_MENU, item)
  end
--local title = Ui_CreateText("Items", "Items", 20, 270, 80, 290, 1.0, 8.0 * HUD_SCALE, false)
  local title = Ui_CreateText("Items", "Items", 20, TITLE_START, 80, TITLE_START + 50, 1.0, 8.0 * HUD_SCALE, false)
  AddResumeButton(INVENTORY_MENU)    
  UiControl_AddChild(INVENTORY_MENU, title)
  UiControl_AddChild(INVENTORY_MENU, INVENTORY_CIRCLE_MENU)
  UiControl_LayoutCircle(INVENTORY_CIRCLE_MENU, INVENTORY_RADIUS)
  UiInventoryHide()
  UiControl_AddChild(CANVAS, INVENTORY_MENU)
end

function GetInventory(player)
  if player == nil then
    return {}
  end
  local data = player:data()
  if data.inventory == nil then
    data.inventory = {}
  end
  return data.inventory
end

function GetInventoryPickupCount(player)
  if player == nil then
    return {}
  end
  local data = player:data()
  if data.inventoryPickupCount == nil then
    data.inventoryPickupCount = {}
  end
  return data.inventoryPickupCount
end

function UiInventoryUpdate(player)
  if INVENTORY_CIRCLE_MENU ~= nil then
    UiControl_LayoutCircle(INVENTORY_CIRCLE_MENU, INVENTORY_RADIUS)
  end
end

function UiInventoryShow()
  if INVENTORY_MENU ~= nil then
    UiControl_SetVisible(INVENTORY_MENU, true)
    PROCESS_BUTTONS = false
  end
end

function UiInventoryHide()
  if INVENTORY_MENU ~= nil then
    UiControl_SetVisible(INVENTORY_MENU, false)
    PROCESS_BUTTONS = true    
  end
end

function RebuildInventoryScreen(player)
  local inv = GetInventory(player)
  -- make them all invisible
  for key, data in pairs(inventoryNames) do
    SetInventoryItemCount(player, key, inv[key])
  end
end

function GetInventoryItemCount(player, item)
  local inv = GetInventory(player)
  if inv[item] == nil then
    inv[item] = 0
  end
  return inv[item]
end

function SetInventoryItemCount(player, item, count)
  local inv = GetInventory(player)
  if count == nil then
    count = 0
  end
  inv[item] = count
  local data = inventoryNames[item]
  local control = data[INVENTORY_UICONTROL]
  if control ~= nil then
    UiControl_SetVisible(control, count ~= 0)
  end
  local text = ""
  if count ~= 1 then
    text = data[INVENTORY_MULTIPLE_PREFIX] .. tostring(count)
  else
    text = data[INVENTORY_SINGLE_PREFIX]
  end
  if control ~= nil then  
    UiControl_SetText(control, text)
  end
end

HEALTH_PICKUP_COUNT = 0
GOLD_PICKUP_COUNT   = 0
GOLD_PICKUP_MOD     = 1

function AddToInventory(player, item, count)
  if count <= 0 or GetPlayer() ~= player then
    return false
  end
  local prevTotal = GetInventoryItemCount(player, item)  
  local total = prevTotal + count
  if prevTotal == 0 then
    local event = "FirstAcquired_" .. item
    FireEvent(event, event)
  end
  SetInventoryItemCount(player, item, total)
  local phrase = ItemPhrase(item, count)
  print("Acquired " .. phrase .. " for a total of " .. total)
  local pickupCounts = GetInventoryPickupCount(player)
  local pickupCount  = pickupCounts[item]
  if pickupCount == nil then
    pickupCount = 0
  end
  pickupCount = pickupCount + 1
  if pickupCount == 1 then
    local itemInfo = inventoryNames[item]
    print("first pickup for " .. item)
    local firstFn = itemInfo[INVENTORY_FIRST_FUNCTION]
    if firstFn ~= nil then
      if firstFn() == false then  -- failed because game is paused somehow, try again next time
        pickupCount = 0
      end
    end    
  end  
  pickupCounts[item] = pickupCount  
  if not IsImmediate(item) then
    if item ~= "Gold" then
      MessageRise("+" .. phrase, 0)
    else
    --[[
      if GOLD_PICKUP_COUNT % 1 == 0 then
        MessageRise("+" .. phrase, 0)
      end
      ]]--
      if GOLD_PICKUP_COUNT % GOLD_PICKUP_MOD == 0 then
        MessageRise("@15\\#ffff40You've Collected Gold!", 0)
        GOLD_PICKUP_MOD = GOLD_PICKUP_MOD * 2
      end
      GOLD_PICKUP_COUNT = GOLD_PICKUP_COUNT + 1      
    end
  else
    MessageRise("\\#40ff40" .. inventoryNames[item][2] .. "\\#ffffff ", 0)
  end
  if IsImmediate(item) then
    local data = inventoryNames[item]
    if data ~= nil then
      InventoryListener("SELECTED", { key = item, execute = data[INVENTORY_FUNCTION] })  
    end
  end
  return true
end

function AddOneToInventory(c, type)
  AddToInventory(c, type, 1)
end

function IsImmediate(item)
  local data = inventoryNames[item]
  if data == nil then
    return false
  end
  return data[INVENTORY_IMMEDIATE]
end

function RetrieveFromInventory(player, item, count)
  if count <= 0 then
    return false
  end
  local total = GetInventoryItemCount(player, item)
  if total - count < 0 then
    print("Don't have enough of ", item, "!")
    return false
  end
  SetInventoryItemCount(player, item, total - count)
  local phrase = ItemPhrase(item, count)
  print("Used " .. phrase .. " with " .. total - count .. " remaining.")
  if not IsImmediate(item) then
    MessageRise("-" .. phrase, 20)
  end
  return true
end

function PrintInventory(player)
  local data = player:data()
  if data.inventory == nil then
    print("No inventory!")
    return
  end
  print("")  
  print("Inventory")
  print("---------")
  for item, count in pairs(data.inventory) do
    print(ItemPhrase(item, count))
  end
  print("---------")
  print("")  
end

local printInventory = 0

function OccasionallyPrintInventory()
--[[
  printInventory = printInventory + 1
  if printInventory % 100 == 0 then
    DumpMemoryStatistics()
    PrintInventory(GetPlayer())
  end
  --]]
end