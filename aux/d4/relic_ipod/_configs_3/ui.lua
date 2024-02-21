print "Lua:ui.lua"

INTRO         = 0
IN_GAME       = 1
INVENTORY     = 2
LOADING       = 3
MAIN_MENU     = 4
TRAILER       = 5
MAX_GAME_MODE = 6

GameMode = IN_GAME

function Touch(x, y)
  print("Touched here: ", x, ", ", y)
end

function DrawInventory()
  DrawBackground(1.0, "inventory")  -- background
  local data = GetPlayer():data()
  local size       = 4.0
  local lineHeight = 3.5  
  local y          = 10.0
  if data.inventory == nil then
    Text_DrawString("No inventory!", 0, y, size, 1.0)
    return
  end
  Text_DrawString("\\#0000ffInventory", 0, y, size, 1.0)
  y = y - lineHeight
  y = y - lineHeight
  for item, count in pairs(data.inventory) do
    Text_DrawString("\\%80800080" .. ItemPhrase(item, count), 0, y, size, 1.0)
    y = y - lineHeight
  end
end

function DrawQuadAt(name, alpha, x, y, width, height)
  x = x - width  / 2
  y = y - height / 2
  DrawQuad(name, alpha, 
                 x, y, 0.0,
                 x + width, y + height, 0.0,
                 1, 0, 
                 0, 1)
end

function DrawAttackButton(player, x, y)
  if _buttons[0] then
    DrawQuadAt("blueringsword_hilite", 1.0, x+8, y-4, 16, 16)
  else
    DrawQuadAt("blueringsword", 1.0, x+8, y-4, 16, 16)
  end
end

function DrawMagicAttackButton(player, x, y)
  x = x - 4
  y = y + 12
  if _buttons[2] then
    DrawQuadAt("magic2_opaq_hilite", 1.0, x, y, 14, 14)
  else
    DrawQuadAt("magic2_opaq", 1.0, x, y, 14, 14)
  end
end

function DrawHealButton(player, x, y)
  x = x - 4
  y = y + 28
  if _buttons[1] then
    DrawQuadAt("blueringfirepotion_opaq_hilite", 1.0, x, y, 14, 14)
  else
    DrawQuadAt("blueringfirepotion_opaq", 1.0, x, y, 14, 14)
  end
end

function DrawPauseButton(player, x, y)
  --x = x - 12
  DrawQuadAt("pause", 1.0, x, y, 8, 8)
end

function DrawStickButton(player, x, y)

  --DrawQuadAt("firering", 0.25, x, y, 24, 24)
  --DrawQuadAt("stick", 1.0, x, y, 16, 16)
  DrawQuadAt("bluering", 0.5, x, y, 24, 24)
  local xi, yi, zi = GetInputDirection()
  local intensity = math.sqrt(xi * xi + yi * yi) / 4.0
  DrawQuadAt("stick2", 1.0 - intensity, x + xi * 1.5, y + yi * 1.5, 16, 16)
  DrawQuadAt("stick2_hilite", intensity, x + xi * 1.5, y + yi * 1.5, 16, 16)  
end

function DrawCombatButtons(player, x, y)
  DrawAttackButton(player, x, y)
  DrawMagicAttackButton(player, x, y)  
 DrawHealButton(player, x, y)    
end

function DrawBackground(alpha, name)
  DrawQuad(name, alpha,
                         -64.2, -42.8, 0.0,
                         64.2, 42.8, 0.0,
                         1, 0, 
                         0, 1)
end

function DrawBackgroundScroll(alpha, name, scroll)
  DrawQuad(name, alpha,
                         -64.2, -42.8, 0.0,
                         64.2, 42.8, 0.0,
                         scroll + 1, 0, 
                         scroll + 0, 1)
end

local startTime = 0

function ResetSequence()
  startTime = 0
end

function DrawSequence(sequence)
  if startTime == 0 then
    startTime = GetTime()
  end
  local count = (GetTime() - startTime) / 1000.0
  local index = math.floor(count + 1)
  local alpha = (count + 1) - index
  index = index + 1
  if index <= #sequence then
    DrawBackground(1.0, sequence[index - 1])
    DrawBackground(alpha, sequence[index])
  else
    ChangeGameMode(MAIN_MENU)
  end
end

local intro = { "Black",
                "intro0", "intro0", "intro0", 
                "intro1", "intro1", "intro1", "intro1", "intro1", "intro1", --"intro1", "intro1", "intro1", "intro1",
                "intro0", 
                --[[
                "intro2", 
                "intro2b", 
                "intro2c", 
                ]]--
                "intro3", "intro3", 
                "intro4", "intro4", "intro4",
                "intro5", "intro5", "intro5", "intro5", "intro5",
                "intro6", "intro6", "intro6", "intro6", "intro6", "intro6", 
                "intro6a", "intro6a", "intro6a", "intro6a", "intro6a", "intro6a", 
                "intro8", 
                "intro7", "intro7", "intro7", 
                "intro8",
                "intro10",
                "intro9", "intro9", "intro9", 
                "intro10", "intro10", 
                "intro10c", "intro10c", 
                "intro11c", "intro11c", "intro11c", "intro11c", "intro11c", "intro11c",
                "intro10c", "intro10c", "intro10c" }

function DrawButton(x, y, length, text)
  local w = 8
  local h = 8
  --[[
  DrawQuad("LongButton", 1.0, x, y, 0.0, x + w, y + h, 0.0, 0, 0, 0.25, 1)
  DrawQuad("LongButton", 1.0, x + w, y, 0.0, x + length, y + h, 0.0, 0.25, 0, 0.5, 1)
  DrawQuad("LongButton", 1.0, x + length, y, 0.0, x + length + w, y + h, 0.0, 0.75, 0, 1, 1)
  Text_DrawStringCentered(text, x + 20, y + 4, 4.0, 1.0)
  --]]
  Text_DrawStringCentered(text, x + 20, y + 4, 7.0, 1.0)
end

function ChangeGameMode(newMode)
  if newMode ~= GameMode then
    if newMode == INTRO then
      ResetSequence()
    end
    if newMode == TRAILER then
      ResetTrailer()
    end
    
    --
  
    GameMode = newMode
  end
end

local scroll1 = -1
local scroll2 =  0
local fire    =  0

function TitleBackground()
    DrawBackground(1.0, "bay")
    DrawBackgroundScroll(0.75, "duskclouds", scroll1)
    DrawBackgroundScroll(0.75, "duskclouds", scroll2)
    scroll1 = scroll1 + 0.002
    if scroll1 > 1 then
        scroll1 = -1
    end
    scroll2 = scroll2 + 0.0007
    if scroll2 > 1 then
        scroll2 = -1
    end
    
    local r1     = math.random()
    local factor = 0.2
    if r1 > 0.90 then
      factor = 0.85    
    elseif r1 > 0.30 then
      factor = 0.5    
    end
    local r2 = math.random() * factor
    if r2 > fire then
      fire = r2
    elseif fire > 0.01 then
      fire = fire - 0.01
    end     
    
    DrawBackground(1.0,  "bayforeground")
    DrawBackground(fire, "bayforegroundfire")
end

function UiUpdateAndRender(instance)
  if GameMode == IN_GAME then
    local player = GetPlayer()
    local x = -52
    local y = -30
    DrawCombatButtons(player, -52, -30)  
  --DrawPauseButton(player, x, 32)
    DrawStickButton(player, 52, -30)
  end
  if GameMode == INTRO then  
    DrawSequence(intro)
  end
  if GameMode == LOADING then      
    Text_DrawString("Loading...", -20, -30, 8.0, 1.0)  
  end
  if GameMode == INVENTORY then
    DrawInventory()
  end
  if GameMode == TRAILER then
    DrawTrailer()
  end  
  if GameMode == MAIN_MENU then
    --DrawBackground(1.0, "intro0")
    TitleBackground()
    --DrawBackground(1.0, "tab")
    DrawQuadAt("RelicInvLogo6", 1.0, 0, 25, 64, 32)
    --DrawBackground(1.0, "inventory")  -- background
    --DrawBackground(1.0, "GlassElement1")
    --DrawQuadAt("GlassElementDS1", 1.0, 0, 0, 80, 80)
    local yincr = 10
    DrawButton(-18, 0,             25, "New Game")
    DrawButton(-18, 0 - yincr,     25, "Load Game")
    DrawButton(-18, 0 - yincr * 2, 25, "Multiplayer")
    DrawButton(-18, 0 - yincr * 3, 25, "Extras")
  end
--DrawQuadAt("RelicInvLogo6", 0.5, -45, -35, 32, 16)  
end