print "lua:inventory.lua"

local inventoryNames =
{
  KeyWhite = { "a",  "White Key",        "s" },
  KeyRed   = { "a",  "Red Key",          "s" },
  KeyBlue  = { "a",  "Blue Key",         "s" },
  Gold     = { "a",  "Gold",             ""  },
  Potion0  = { "a",  "Red Potion",       "s" },
  Potion1  = { "a",  "Blue Potion",      "s" },
  Potion2  = { "a",  "Green Potion",     "s" },
  Potion3  = { "a",  "Yellow Potion",    "s" },
--  Potion0  = { "a",  "\129",       "s" },
--  Potion1  = { "a",  "\132",      "s" },
--  Potion2  = { "a",  "\131",     "s" },
--  Potion3  = { "a",  "\130",    "s" },
  Potion4  = { "a",  "Violet Potion",    "s" },
  Potion5  = { "an", "Orange Potion",    "s" },
--  Potion5  = { "an", "\129",    "s" },
  Potion6  = { "a",  "Teal Potion",      "s" },
  Potion7  = { "a",  "White Potion",     "s" },
  Potion8  = { "a",  "Black Potion",     "s" },
  Book0    = { "a",  "Yellow Spellbook", "s" },
  Book1    = { "a",  "Red Spellbook",    "s" },
  Book2    = { "a",  "Violet Spellbook", "s" },
  Book3    = { "a",  "Blue Spellbook",   "s" },
  Book4    = { "a",  "Green Spellbook",  "s" }
}

function ItemPhrase(item, count)
  local name = inventoryNames[item]
  if count == 0 then
    return "no " .. name[2] .. name[3]
  elseif count == 1 then
    return name[1] .. " " .. name[2]
  end
  return count .. " " .. name[2] .. name[3]
end

function AddToInventory(player, item, count)
  local data = player:data()
  if data.inventory == nil then
    data.inventory = {}
  end
  local inv = data.inventory
  if inv[item] == nil then
    inv[item] = 0
  end
  inv[item] = inv[item] + count
  local phrase = ItemPhrase(item, count)
  print("Acquired " .. phrase .. " for a total of " .. inv[item])
  SendRisingMessage("+" .. phrase, 0)
end

function AddOneToInventory(c, type)
  AddToInventory(c, type, 1)
end

function RetrieveFromInventory(player, item, count)
  local data = player:data()
  if data.inventory == nil then
    return false
  end
  local inv = data.inventory
  local invItem = inv[item]
  if invItem == nil or invItem <= 0 then
    print("Don't have a(n) ", item, "!")
    return false
  end
  inv[item] = invItem - count
  local name = ItemPhrase(item, count)  
  print("Used " .. name .. " with " .. inv[item] .. " remaining")
  SendRisingMessage("-" .. name, 20)
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

  printInventory = printInventory + 1
  if printInventory % 100 == 0 then
    DumpMemoryStatistics()
    PrintInventory(GetPlayer())
  end
end