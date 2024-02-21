print "Lua:trailer.lua"

function NewXSequence(sequence, initialTime, delay)
  local state = {}
  state.sequence = sequence
  state.done = false
  state.startTime = 0
  state.initialTime = initialTime
  state.delay = delay
  return state  
end

function DrawXSequence(state)
  if state.done then
    return
  end
  if state.startTime == 0 or state.startTime == nil then
    state.startTime = GetTime()
  end
  local count = ((GetTime() + state.initialTime) - state.startTime) / state.delay
  if count < 0 then
    count = 0
  end
  local index = math.floor(count + 1)
  local alpha = (count + 1) - index
  index = index + 1
  if index <= #state.sequence then
    if index > 0 then
      DrawBackground(1.0 - alpha, state.sequence[index - 1])
    end
    DrawBackground(alpha, state.sequence[index])
  else
    state.done = true
  end
end

local pics = { "Black", "Black",
               "Black", "Black",
               --"trailerPic1", "trailerPic1", 
               "trailerPic1b", --"trailerPic1b",
               "trailerPic1b", "trailerPic1b", 
               --"trailerPic2", 
               --"trailerPic2b",
               "trailerPic2c",               
               "trailerPic2c",
               "trailerPic2c",
"Black", "Black", "trailerPic3b",               
               --"trailerPic2c",
               --"trailerPic2c",
               --"trailerPic3", 
               "trailerPic3b",
               "trailerPic3b", "trailerPic3b", 
               "Black", "Black", "Black", "Black", "Black", "Black", "Black", "Black" }
               
local text = { "Black",

               "trailerText1", "trailerText1","trailerText1", "trailerText1", "blank",
               "trailerText2","trailerText2", "trailerText2", --"trailerText2", 
               "blank", 
"Black", "Black", "trailerPic3b",               
               "trailerText3", 
               "trailerText3", "trailerText3", "blank",
               
               "trailerText4", "trailerText4", "trailerText4", "trailerText4", "blank",
               
               "trailerTitle0a", 
               --"trailerTitle0", 
               "trailerTitle1", "trailerTitle2", "trailerTitle2",
               "blank", "blank", "ax2", "chillingo", "chillingo", "chillingo", "chillingo"  }               

local trailerPics  = nil
local trailerTexts = nil

function ResetTrailer()
  trailerPics  = NewXSequence(pics, 0, 2000)
  trailerTexts = NewXSequence(text, -1000, 2000)
end

function DrawTrailer()

  if (trailerPics == nil or trailerTexts == nil) or (trailerPics.done and trailerTexts.done) then
    return
  end

  DrawBackground(1.0, "Black")
  DrawXSequence(trailerPics)
  DrawXSequence(trailerTexts)

--  TitleBackground()

end