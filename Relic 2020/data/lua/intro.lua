print "Lua:intro.lua"

WIDTH  = _G.UI_RIGHT
HEIGHT = _G.UI_BOTTOM

INITIAL_TIME        = nil
UI_TIME             = nil
INITIAL_LOADED_TIME = nil
STARTED_TIME        = nil

INTRO_SEQUENCE_CROSS_FADE_TIME = 1500
INTRO_SEQUENCE_END_TIME        = 40000

function GetUiTime()
  if INITIAL_TIME == nil then
    INITIAL_TIME = GetTime()
  end
  UI_TIME = GetTime()
  return UI_TIME - INITIAL_TIME
end

function FadeInAlpha_UiTime()
  local fadein = 3000
  local time = GetUiTime()
  local alpha = 1.0
  if time < fadein then
    alpha = time / fadein
  end
  return alpha
end

function ChooseImage(lores, hires)
  local image = lores
  if IsHiResDevice() then image = hires; end
  return image
end

function FadeInAlpha_StartedTime()
  local fadein = 3000
  local time = GetStartedTime()
  local alpha = 1.0
  if time < fadein then
    alpha = time / fadein
  end
  return alpha
end

function GetStartedTime()
  if not IS_STARTED then
    return 0
  end
  if INITIAL_LOADED_TIME == nil then
    INITIAL_LOADED_TIME = GetTime()
  end
  STARTED_TIME = GetTime()
  return STARTED_TIME - INITIAL_LOADED_TIME
end

IsIntroSequenceDone_HAS_INTRO_SEQUENCE_EVER_COMPLETED = false

function IsIntroSequenceDone()
  if IsIntroSequenceDone_HAS_INTRO_SEQUENCE_EVER_COMPLETED then return true end
  local value = GetStartedTime() >= 56000 or TOUCHED
  if value then IsIntroSequenceDone_HAS_INTRO_SEQUENCE_EVER_COMPLETED = true end
  return value
end

SEGMENTED_HORIZONTAL_OFFSET = 0

function DrawSegmentedHorizontal(file, alpha, offset)
  local overallScale = 1.0
  local offsetHeight = (HEIGHT - HEIGHT * overallScale) * 0.5
  local width = 1024 * 1.25 -- adjust for the fact that we're mapping a 256 height piece of texture to 320px
  local offsetWidth = offset * 4096 * (1 / 1.25)
  SEGMENTED_HORIZONTAL_OFFSET = offsetWidth
  Ui_DrawScreenPanorama(file, alpha, offset, 0)
end

function DrawCanvas(alpha)
  if not IsHiResDevice() then return; end
  local x, y, w, h = GetUiBounds()
  local offset = SEGMENTED_HORIZONTAL_OFFSET - x
  for i = 0, 8192, 512 do 
    Ui_DrawQuad("canvas", 0.11 * alpha,
                0   - offset + i, -128, 0.0,
                512 - offset + i,  512, 0.0,
                --0.5, 0, 0, 0.5)
				1.0, 0, 0, 1.0)
  end
end

local INTRO_TEXT =
{
  { text   = "",
    y      = 82,
    time   = 0 },
    
  { text   = "Long ago, craftsmen of\nprofound art conceived\nan artifact of\nimmortality:",
    y      = 100,
    time   = 1000 },
    
  { text   = "an engine of life to\noverthrow death and\ncommence the rule of\npeace.",
    y      = 100,
    time   = 10000 },    
    
  { text   = "Yet eons later,\ndeath and chaos reign\nas the holes of the\nearth fill with\nhorrors.",
    y      = 80,
    time   = 21000 },      

  { text   = "",
    y      = 82,
    time   = 30000 },
    
  { text   = "",
    y      = 82,
    time   = 32000 },    
}

--[[
x Part I: (pre-start) Long ago, craftsmen of profound art conceived an artifact of immortality.  An engine of life to overthrow death and commence the rule of peace.  Yet 1,000 death and chaos reign as the holes of the earth fill with horrors.
Part II: (level 1-2): The mysterious apparition Aos warns you.  "The levels of the dungeon hold Forges of Souls and each is a piece of The Relic.  The Quinotaur, a scourge of great evil, uses The Relic to subdue the dungeon and the world.
Part III: (level 2-3): "This world began in cataclysm.  I was there; I am responsible.  Now, the Quinotaur rules my fate.
Part IV: (level 3-4): "The ancients taught machines to learn.  They learned to never die and to create life.  Then they devised a machine to better create life that kills.  Life craves two things: to live and to control, and there are always those who kill to control.
Part V: (level 4-5): "**I created you**, your companions and others - such as the Quinotaur - to destroy me.  All failed, fallen to the sword or to corruption, but you have not.  **Destroy the monster, the machine and release me.** (Quinotaur killed)
Part VI: (level 5-6): "I am the Amaranth Operating System v1.0 (Aos) Genome Processor running on the RLC-.. platform.  I am the soul of a machine.  I am the weapon known as The Relic and I destroyed the world of old.
"After centuries of scanning tissue, modeling their internal processes and making the human and animal plan (genome) more lethal and murderous, I **cannot help** but be overcome by the desires and passions of living things.
Part V: (end): Aos flickers out: "**You are my greatest creation.**  Thank you.." (as rising text or dialog from the machine?)
The Relic is destroyed; the Quinotaur's oppressive reign is over.  Go now, ascend to a world you have never seen: a world ravaged by a millennium of terror.
]]--

QUINOTAUR_DESCRIBE_SEQUENCE_1 = 2
CATACLYSM_SEQUENCE            = 3
RELIC_WEAPON_SEQUENCE         = 4
QUINOTAUR_SEQUENCE_1          = 5
LAST_LEVEL_SEQUENCE           = 6
COMPLETED_GAME_SEQUENCE       = 7
START_SEQUENCE                = 8
GAME_OVER_SEQUENCE            = 9
GET_RING_SEQUENCE             = 10
GENERATOR_DESTROYED_SEQUENCE  = 11
GENERATOR_QUEST_SEQUENCE      = 12
TITLE_SEQUENCE                = 13
KEY_ASCEND_STAIRS_SEQUENCE    = 14
ARACHNAREX_SEQUENCE           = 15
ORACLE_SEQUENCE               = 16
WE_ARE_BROTHERS_SEQUENCE      = 17
MARKETING_SEQUENCE            = 18

function ShowMapTipIfNotTryLater()
  if ShowCharacterTip() == false then
    DeferByTimeMs(2500, "ShowMapTipIfNotTryLater")
  end
end

function OnSurviveEndFunction()
  DeferByTimeMs(2500, "ShowMapTipIfNotTryLater")
end

function SequenceFlash()
  Flash(1000)
end

function X()
  print("**X**")
end

local TEXT_PARTS =
{
{
  { sequenceend = 2000 }, -- 1: x
  { y = 100, time = 1000,  text = "Long ago, craftsmen of\nprofound art conceived\nan artifact of\nimmortality;" },
  { y = 100, time = 10000, text = "An engine of life to\noverthrow death and\ncommence the rule of\npeace." },    
  { y = 80,  time = 21000, text = "Yet 1,000 years death\nand chaos reign\nas the holes of the\nearth fill with\nhorrors." },      
  { y = 82,  time = 30000, text = "" },
  { y = 82,  time = 32000, text = "" },    
},
{
  { sequenceend = 30000, darkness = 0.5, background = false, images = false, lockinput = true }, -- 2: x
  { y = 100, time = 1000,  text = "The apparition \\#808080Aos\\#ffffff\nwarns you, \n\\#808080\"Each \\#ffffffForge of Souls\\#80ff80\nis part of \\#ffffffThe Relic\\#808080.\"" },
  { y = 80,  time = 10000, text = "\\#808080\"The \\#ffffffQuinotaur\\#808080, a\nscourge of great evil,\nuses \\#ffffffThe Relic\\#808080 to\nsubdue the dungeon\n- and the world.\"" },
  { y = 82,  time = 20000, text = "\\#808080\"He calls himself the\nfive horned beast:\nOne for each brother\nhe slew to rule alone.\"" },
  { y = 82,  time = 30000, text = "" },
  { y = 82,  time = 35000, text = "" }
},
{
  { sequenceend = 18000, darkness = 0.5, background = false, images = false, lockinput = true }, -- 3: x
  { y = 120, time = 1000,  text = "\\#80ff80Your world began in\ncataclysm.  I was there.\nI am responsible." },
  { y = 140, time = 8000,  text = "\\#80ff80Now, the \\#ffffffQuinotaur\\#80ff80\nrules my fate." },
  { y = 82,  time = 15000, text = "" },
  { y = 82,  time = 20000, text = "" },  
},
{
  { sequenceend = 45000, background = true, image = "panorama_segmented3", scrollspeed = 0.25, lockinput = true },  -- 4: x
  { y = 160, time = 1000,  text = "The phantom speaks:" },  
  { y = 120, time = 6000,  text = "\\#808080The ancients learned\nto never die and\n- to create life." },
  { y = 80,  time = 14000, text = "\\#808080They devised a machine\n- a weapon -\nto create a kind\nof life that kills." },
  { y = 140, time = 20000, text = "\\#808080That weapon is\n\\#ffffffThe Relic\\#808080." },
  { y = 140, time = 27000, text = "\\#808080It destroyed\nthe world of old." },
  { y = 82,  time = 30000, text = "" },
  { y = 140, time = 32000, text = "\\#808080Now, the \\#ffffffQuinotaur\\#808080\nrules \\#ffffffThe Relic" },
  { y = 160, time = 40000, text = "\\#808080And our fates." },
  { y = 82,  time = 45000, text = "" },
},
{
  { sequenceend = 30000, darkness = 0.5, background = false, images = false, lockinput = true }, -- 5: x
  { y = 100, time = 1000,  text = "\\#808080I created the\n\\#ffffffQuinotaur\\#808080\nand his brethren -\nto destroy me." },
  { y = 80, time = 10000, text = "\\#808080All failed,\nfallen to the sword\nor to corruption.\n\nYet you have not." },
  { y = 100, time = 20000, text = "\\#808080Destroy the monster\nnow,\nthe machine and\nrelease us." },
  { y = 82,  time = 30000, text = "" },
  { y = 82,  time = 40000, text = "" },
},
{
  { sequenceend = 70000, darkness = 0.5, background = true, image = "relic_history_panorama6", scrollspeed = 0.5, lockinput = true, pause = true  },  -- 6: x
  { y = 160, time = 1000,  text = "Aos appears again:" },  
  { y = 100, time = 5000,  text = "\\#808080I am an instance of the\n\\#ffffffAmaranth Operating\nSystem v0.8 (\\#808080Aos\\#ffffff)\nBiological Processor." },
  { y = 140, time = 11000, text = "\\#808080I am the soul of a\nmachine." },
  { y = 80, time = 15000, text = "\\#808080The soul of the weapon\nknown as \\#ffffffThe Relic\\#808080.\n\nI destroyed the world\nof old." },
  { y = 82,  time = 25000, text = "" },  
  { y = 100, time = 27000, text = "\\#808080After centuries of\nscanning tissue and\nmaking the human plan\nmore lethal," },
  { y = 100, time = 37000, text = "\\#808080I have become captivated\nby my errors -\nmy creations that seek\nto \\#ffffffcreate\\#808080, not destroy.." },
  { y = 100, time = 47000, text = "\\#808080I cannot help but\nbe overcome by the\ndesires and passions\nof living things." },
  { y = 140, time = 60000, text = "\\#ff4040And I must die\nso that you may live." },
  { y = 82,  time = 70000, text = "" },  
},
{
  { sequenceend = 45000, darkness = 0.5, background = true, image = "relic_final1_panorama2", lockinput = true, pause = true }, -- 7: x
  { y = 100, time = 1000,  text = "Aos flickers out:\n\\#808080You are my greatest\ncreation." },
  { y = 82,  time = 5000,  text = "" },  
  { y = 80,  time = 10000, text = "The \\#808080Relic\\#ffffff is destroyed.\n\nThe Quinotaur's\noppressive rule is\nover.\n" },
  { y = 120, time = 20000, text = "Go now, ascend to a\nworld you have never\nseen:" },  
  { y = 120, time = 25000, text = "A world ravaged\nby millennia of\nterror." },
  { y = 82,  time = 35000, text = "" },
  { y = 82,  time = 40000, text = "" },
},
{
  { sequenceend = 12000, darkness = 0.5, background = false, images = false, lockinput = true, onendfunction = OnSurviveEndFunction }, -- 8: x
  { y = 160, time = 0,  text = "" },    
  { y = 160, time = 1000,  text = "Survive." },  
  { y = 160, time = 5000,  text = "" }, 
  { y = 140, time = 7000,  text = "Seek the \\#808080phantom\\#ffffff to\nguide you." },
  { y = 82,  time = 12000, text = "" },
  { y = 82,  time = 20000, text = "" },
},
{
  { sequenceend = 20000, darkness = 0.5, background = false, images = false, lockinput = true, pause = false, nonskippable = true },  -- 9: x
  { y = 160, time = 1000,  text = "death reigns yet" },
  { y = 82,  time = 8000,  text = "" },
  { y = 82,  time = 20000, text = "" },
},
{
  { sequenceend = 27000, darkness = 0.5, background = false, images = false, lockinput = true, pause = true },  -- 10: x
  { y = 140, time = 1000,  text = "The phantom \\#808080Aos\\#ffffff\nhaunts this place:" },
  { y = 140, time = 6000,  text = "To escape you must\ndestroy \\#808080The Relic\\#ffffff." },
  { y = 120, time = 10000,  text = "To reach \\#808080The Relic\\#ffffff\nyou will need magic\nand strength." },
  { y = 120, time = 15000, text = "Find the magic\n\\#808080Amaranth Ring\\#ffffff\nin a nearby room." },
  { y = 160, time = 20000, text = "Take this key." },
  { y = 82,  time = 25000, text = "" },
  { y = 82,  time = 35000, text = "" },
},
{
  { sequenceend = 17000, darkness = 0.5, background = false, images = false, lockinput = true, pause = true },  -- 11: x
  { y = 120,  time = 1000,  text = "Excellent. You\ndestroyed the first\n\\#808080Forge of Souls!\\#ffffff" },
  { y = 140,  time = 6000,  text = "Each \\#808080Forge of Souls\\#ffffff\nis part of \\#808080The Relic\\#ffffff." },
  { y = 140,  time =11000,  text = "You have given me cause\nfor hope." },
  { y = 82,  time = 17000,  text = "" },
  { y = 82,  time = 30000, text = "" },
},
{
  { sequenceend = 20000, darkness = 0.5, background = false, images = false, lockinput = true, pause = true },  -- 12: x
  { y = 120, time = 1000, text = "You must destroy the\n\\#808080Forge of Souls\\#ffffff\nin the atrium." },
  { y = 140, time = 7000, text = "It is not far. Here is a key\nto enter that room." },
  { y = 82,  time = 25000, text = "" },
  { y = 82,  time = 30000, text = "" },
},
{
  { sequenceend = 100000000, background = true, image = "panorama_segmented3", scrollspeed = 0.5, overlayalpha = 0.5, nonskippable = true },  
  { y = 120, time = 1000, text = "" },
  { y = 120, time = 5000, text = "" },
},
{
  { sequenceend = 7000, darkness = 0.5, background = false, images = false, lockinput = true, pause = true },  
  { y = 100, time = 1000,  text = "Use the \\#808080Amaranth\nRing\\#ffffff wisely.\n\nAscend the stairs.\nI will await you there." },
  { y = 82,  time = 8000,  text = "" },
  { y = 82,  time = 20000, text = "" },
},
{
  { sequenceend = 7000, darkness = 0.5, background = false, images = false, lockinput = true, pause = true },  
  { y = 100, time = 1000,  text = "The spider \\#808080Arachnarex\\#ffffff\nguards the way.\n\nDestroy it and ascend." },
  { y = 82,  time = 8000,  text = "" },
  { y = 82,  time = 20000, text = "" },
},
{
  { sequenceend = 7000, darkness = 0.5, background = false, images = false, lockinput = true, pause = true },  
  { y = 120, time = 1000,  text = "Find \\#808080Oracles\\#ffffff\n to explain what\nis unclear." },
  { y = 82,  time = 8000,  text = "" },
  { y = 82,  time = 20000, text = "" },
},
{
  { sequenceend = 20000, darkness = 0.5, background = false, images = false, lockinput = true }, -- 5: x
  { y = 160,  time = 1000,  text = "The Quinotaur speaks:" },
  { y = 80, time = 3000,  text = "\\#ff0000STOP!\n\nYou and I,\n\nWE ARE BROTHERS!" },
  { y = 82,  time = 20000, text = "" },
  { y = 82,  time = 30000, text = "" },
},
{
  { sequenceend = 100000, background = true, image = "black", scrollspeed = 0.25, lockinput = true },  -- 4: x
  { y = 160, time =  1000,  text = "" },  
  { y = 100, time =  6000,  text = "Ascend from deep in\nthe dungeons on a\nquest to destroy\nThe Relic." }, 
  { y = 160, time = 11000,  text = "" },  
  { y = 140, time = 16000,  text = "Skeletons, spiders\nand wizards await." },
  { y = 160, time = 21000,  text = "" },
  { y = 120, time = 26000,  text = "Collect gold.\nKeep mana for\npowerful magic." },
  { y = 160, time = 31000,  text = "" },
  { y = 120, time = 36000,  text = "Enter a deep world of\nadventure and mystery\nin The Relic." },
  { y = 160, time = 41000,  text = "" },
  { y = 120, time = 46000,  text = "Rack up kills\nand purchase special\nabilities." },
  { y = 160, time = 51000,  text = "" },    
  { y = 140, time = 56000,  text = "Uncover the mystery\nof The Relic." },
  { y = 160, time = 61000,  text = "" },
  { y = 100, time = 66000,  text = "Hit rapidly to boost\nyour hit multiplier.\n\nUnleash powerful attacks." },
  { y = 160, time = 71000,  text = "" },  
},
}

function WriteSequenceTextItem(item, alpha)
  local x, y, w, h = GetUiBounds()
  if item == nil then
    return
  end
  local text   = item.text
  local y      = item.y
  Ui_DrawStringCenteredGlow(text, w * 0.5 + x, y, 40, alpha * 1.0)
end

function GetCurrentSegmentIndex(text, time, initialTime)
  local ms     = GetStartedTime() - initialTime
  local prev   = nil
  local item   = nil
  local index  = 0
  for i = 1, #text do
    item = text[i]
    if item == nil or item.time == nil then
      break
    end
    if ms < item.time then
      if i > 1 then i = i - 1 end
      item = text[i]
      index = i
      if i > 1 then i = i - 1 end
      prev = text[i]
      break
    end
    item = nil
  end
  return index
end  

function WriteSequenceText(text, time, initialTime)
  local trans  = INTRO_SEQUENCE_CROSS_FADE_TIME
  local ms     = GetStartedTime() - initialTime
  local prev   = nil
  local item   = nil
  for i = 1, #text do
    item = text[i]
    if item == nil or item.time == nil then
      break
    end
    if ms < item.time then
      if i > 1 then i = i - 1 end
      item = text[i]
      if i > 1 then i = i - 1 end
      prev = text[i]
      prev._inprogress = nil
      break
    end
    item = nil
  end
  if item ~= nil and item.time ~= nil then
  
    -- execute a function at a given stage
    if item._inprogress ~= true then
      item._inprogress = true
      if item.func ~= nil then
        print ("** function **")
        local fn = item.func
        print ("*fn* : " .. tostring(fn))
        fn()
      end    
    end
  
    if ms - item.time < trans then
      local transition = (ms - item.time) / trans
      WriteSequenceTextItem(prev, (1 - transition) * time)
      WriteSequenceTextItem(item, transition * time)
    else
      WriteSequenceTextItem(item, time)
    end
  end
end

function WriteIntroText(time)
  WriteSequenceText(INTRO_TEXT, time, 0)
end

function GetIntroSequenceAlpha()
  local ms   = GetStartedTime()
  local fade = ms - INTRO_SEQUENCE_END_TIME
  if fade < 0 then
    return FadeInAlpha_StartedTime()
  elseif fade > INTRO_SEQUENCE_CROSS_FADE_TIME then
    return 0
  else
    return 1 - (fade / INTRO_SEQUENCE_CROSS_FADE_TIME)
  end
end

function GetTitleAlpha(time, fadeTime)
  local ms   = GetStartedTime()
  local fade = ms - time
  if fade < 0 then
    return 0
  elseif fade > fadeTime then
    return 1
  else
    return (fade / fadeTime)
  end
end

SHOW_MENUS_CALLED = false
SEQUENCE_START = { nil, nil, nil, nil, 
                   nil, nil, nil, nil, 
                   nil, nil, nil, nil, 
                   nil, nil, nil, nil }

function DrawIntroBackground()

--DrawBackgroundPartial(GetTitleAlpha(3000, 4500), "background_partial")   
end

function DrawIntroSequence()

  local bx, by, bw, bh = GetUiBounds()

  PROCESS_BUTTONS = false
  DISABLE_UI_INPUT = true
  local alpha = GetIntroSequenceAlpha()
  local titleAlpha1 = GetTitleAlpha(43000, 1500)
  local titleAlpha2 = GetTitleAlpha(45000, 1500)  
  local titleAlpha3 = GetTitleAlpha(47000, 1500)
  local titleAlpha4 = GetTitleAlpha(49000, 1500)  
  local titleAlpha5 = GetTitleAlpha(51000, 1500)
--DrawIntroBackground()
  DrawBackgroundPartial(FadeInAlpha_UiTime(), "background_partial_x2")
  
  if alpha > 0 then
    local offset = (1 - (math.cos(GetStartedTime() * 0.00003 * 3.1415926) + 1) * 0.5)
    DrawSegmentedHorizontal("panorama_segmented", alpha * 0.99, offset)
    DrawBackground(FadeInAlpha_UiTime() * 0.8, "intro10c_map_256sq")
    DrawCanvas(alpha)
    WriteIntroText(alpha)    
  elseif titleAlpha1 > 0 or titleAlpha2 > 0 then
    DrawBackgroundPartial(titleAlpha1, "trailerTitle0a_partial_X2")
    DrawBackgroundPartial(titleAlpha2, "trailerTitle2_partial_x2")
    DrawBackgroundPartial(titleAlpha3, "trailerTitle2_partial_x2")
    DrawBackgroundPartial(titleAlpha4, "trailerTitle2_partial_x2")
    if titleAlpha5 >= 1 and not SHOW_MENUS_CALLED then
      SHOW_MENUS_CALLED = true
    --ShowMainMenu()
    --StartGameSession(1)
      SetGameStateId(GAMESTATE_MENU)
      UnloadSequenceTextures()
    end
  end
end

function ClearSequenceStarts()
  for i = 1, GetSequenceCount() do
    SEQUENCE_START[i] = nil
  end
end

function EnableUiInput()
  ClearButtonPresses()
  DISABLE_UI_INPUT = false
end

function EndSequence()
  PROCESS_BUTTONS = false
  ClearSequenceStarts()
  UnloadSequenceTextures()
  SetPaused(false)
  SetRenderPaused(false)
  SetSuppressHud(false)
  DRAW_SEQUENCE_INDEX = nil
  PROCESS_BUTTONS = true
  LOCK_INPUT = false
  ClearButtonPresses()
  DeferByTimeMs(500, "EnableUiInput") -- wait a beat before accepting input
end

function GetSequenceCount()
  return #TEXT_PARTS
end

function StartSequence(index)
  print("***starting sequence", index, "***")
  PROCESS_BUTTONS = false
  EndSequence()
  DRAW_SEQUENCE_INDEX = nil
  if index > 0 then
    DoFlash(2000)
    Audio_PlayAt("#vibrate", 0, 0, 0, 1.0)
    local data = TEXT_PARTS[index]
    data[1].text = ""
    data[1].time = 0
    data[1].y = 0
    local background = data[1].background
    local pause = data[1].pause
    if pause ~= false then pause = true end
    SetRenderPaused((background ~= false and pause))
    SetPaused(pause)
    if data[1].lockinput == true then
      ClearButtonPresses()
      LOCK_INPUT = true
    end
    SetSuppressHud(true)
    DRAW_SEQUENCE_INDEX = index
  end
end

function DrawSequence()
  if DRAW_SEQUENCE_INDEX ~= nil then
    DrawSequenceByIndex(DRAW_SEQUENCE_INDEX)
  end
end

function DrawSequenceByIndex(index)
  if SEQUENCE_START[index] == nil then
    SEQUENCE_START[index] = GetStartedTime()
  end
  local data          = TEXT_PARTS[index]
  if data == nil then
    return
  end
  local background    = data[1].background
  local darkness      = data[1].darkness
  local images        = data[1].images
  local image         = data[1].image
  local scrollspeed   = data[1].scrollspeed
  local sequenceend   = data[1].sequenceend
  local overlayalpha  = data[1].overlayalpha
  local nonskippable  = data[1].nonskippable
  local onendfunction = data[1].onendfunction
  
  -- handle background and darkness
  if background ~= false then
    DrawBackgroundPartial(1.0, "background_partial_x2")
  elseif darkness ~= nil then
    DrawBackground(darkness, "Black") -- we use this instead of SetDarkness() so that it shows up behind..
    --SetDarkness(darkness)
  end
  
  local PANORAMA = false
  
  -- handle panorama images
  if images ~= false then
    if scrollspeed == nil then
      scrollspeed = 1.0
    end
    local offset = (1 - (math.cos(((GetStartedTime() - SEQUENCE_START[index]) * scrollspeed) * 0.00003 * 3.1415926) + 1) * 0.5)
    if image == nil then
      image = "panorama_segmented2"
    end
    DrawSegmentedHorizontal(image, 1.0, offset)
    PANORAMA = true
  end
  
  -- write text
  WriteSequenceText(data, 1.0, SEQUENCE_START[index])
  if images ~= false then    
    if overlayalpha == nil then
      overlayalpha = 1.0
    end
    DrawBackground(0.8 * overlayalpha, "intro10c_map_256sq")
  end
  DrawCanvas(1.0)
  if SEQUENCE_START[index] ~= nil and LOW_LEVEL_TOUCHED == true and nonskippable ~= true then
    local thisTime = GetStartedTime()
    local segment = GetCurrentSegmentIndex(data, thisTime, SEQUENCE_START[index])
    if segment + 1 <= #data then
      local thisItemTime = data[segment].time + SEQUENCE_START[index]
      if thisTime - thisItemTime > 1000 then -- require at least 1s
        local nextItemTime = data[segment + 1].time + SEQUENCE_START[index]
        local addTime = nextItemTime - thisTime
        SEQUENCE_START[index] = SEQUENCE_START[index] - (addTime + INTRO_SEQUENCE_CROSS_FADE_TIME)
      end
      LOW_LEVEL_TOUCHED = false      
    end
  end
  if GetStartedTime() - SEQUENCE_START[index] > sequenceend or (nonskippable ~= true and LOW_LEVEL_TOUCHED == true) then
    DoFlash(4000)
    EndSequence()
    if onendfunction ~= nil then
      onendfunction()
    end
  end
end

function UnloadSequenceTextures()
  UnloadTexture("relic_final1_panorama")
  UnloadTexture("relic_history_panorama2")
  UnloadTexture("relic_history_panorama3")
  UnloadTexture("crystal")  
  UnloadTexture("panorama_segmented")
  UnloadTexture("panorama_segmented2")
  UnloadTexture("panorama_segmented3")
  UnloadTexture("relic_history_panorama6")
  UnloadTexture("relic_final1_panorama2")
  UnloadTexture("background_partial_x2")
  UnloadTexture("chillingo_partial")
  UnloadTexture("trailerTitle0a_partial")
  UnloadTexture("trailerTitle0a_partial_X2")  
  UnloadTexture("trailerTitle1_partial")
  UnloadTexture("trailerTitle2_partial")
  UnloadTexture("trailerTitle2_partial_x2")  
  UnloadTexture("trailerTitle3_partial")
  UnloadTexture("instructions2_partial")
  UnloadTexture("RelicLogoInGame")
  UnloadTexture("canvas")
  UnloadTexture("RelicLogoInGame_x2")  
  UnloadTexture("intro10c")
  UnloadTexture("intro11c")
  UnloadTexture("intro11d")
  UnloadTexture("keySq128Green")
  UnloadTexture("keySq128Yellow") 
end
