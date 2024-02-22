print "lua:savepoint.lua"

function SavePoint(c)
end

STORY_DEATH_STING = {  "O death,",  "where is thy sting?",  "O grave,",  "where is thy victory?" }
STORY_ONCE_AGAIN  = {  "That day before",  "they shut the lid",  "I took a flaxen hair",  "from her cotton dress",
                       "For by God's grace",  "and men's machines",  "I'll love her",  "once again" }
STORY_FIVE_HORNS  = {  "The Quinotaur calls himself",  "the five horned beast.",
                       "One horn for each brother",  "he slew to rule alone." }
STORY_QUINOTAUR   = {  "The Quinotaur,",  "a ruler of great evil,",  "uses The Relic to",  "subdue this dungeon",  "- and the world!" }
STORY_ANCIENTS1   = {  "The ancients learned",  "to never die and",  "- to create life." }
STORY_ANCIENTS2   = {  "Men of old",  "devised a machine",  "to create a species",  "of life that kills:",  "An effective weapon." }

BOOK_STORIES      = {  STORY_QUINOTAUR,  STORY_ANCIENTS1,  STORY_ANCIENTS2,  STORY_ONCE_AGAIN,  STORY_DEATH_STING,  STORY_FIVE_HORNS, }

BOOK_STORY_INDEX = 1

function SavePointListener(info, data)
  local time = GetTime()
  if LAST_BOOK_TIME ~= nil and time < LAST_BOOK_TIME + 5000 then
    return
  end
  LAST_BOOK_TIME = time
  local message = BOOK_STORIES[BOOK_STORY_INDEX]
  BOOK_STORY_INDEX = (BOOK_STORY_INDEX % #BOOK_STORIES) + 1
  if message ~= nil then
    MessageRise("", 0)
    MessageRise("@15\\#a0a0a0The \\#ffffffOracle Device\\#a0a0a0 replies -", 0)  
    for i = 1, #message do
      MessageRise("", 0)
      MessageRise("@15\\#80ff80" .. message[i] .. "\\#ffffff ", 0)
    end
  end
end

function SaveHudListener(info, data)
  if info == 'SELECTED' then
    ShowSaveExitMenu()
  end
end
--[[
function LoadGameListener(info, data)
  if info == 'SELECTED' then
    HideGenericDialog()
    LoadPrimaryPlayer()
  end
end

function SaveGameListener(info, data)
  if info == 'SELECTED' then
    HideGenericDialog()
    SavePrimaryPlayer()
  end
end
]]--

function CreateSavePoint_Character(c, type, hp, speed, x, y)
  c:SetFixed(true)
  c:SetStatic(true)
  SetSavePointHaloEffect(c)
  SetAttributes(c, 10000.0, 0)
  c:SetScale(1.1 * characterScale)
  c:SetRadius(characterRadius)
  c:data().selectEvent = "Select_SavePoint"
  c:data().selector = InteractSelector
end

function CreateSavePoint(type, hp, speed, x, y)
  local c = NewCharacter(-1, "SavePoint", type, x, y, 0.0)
  CreateSavePoint_Character(c, type, hp, speed, x, y)
  return c
end
