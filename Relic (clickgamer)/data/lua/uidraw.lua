print "Lua:uidraw.lua"

MESSAGE_RISE = true

function MessageRise(text, delay)
  if MESSAGE_RISE then
    SendRisingMessage(text, delay)
  end
end

function DrawQuadAt(name, alpha, x, y, w, h)
  x = x - w / 2
  y = y - h / 2
  Ui_DrawQuad(name, alpha,
              240 - x,   160 - y, 0.0,
              240 - (x + w), 160 - (y + h), 0.0,
              1, 0,
              0, 1)

end

function DrawBackground(alpha, name)
  if GetPlatformIsiPad() then
    Ui_DrawQuad(name, alpha,
				-30, -20, 0.0,
                510, 340, 0.0,
                0, 1, 
                1, 0)
  else
    Ui_DrawQuad(name, alpha,
                  0,   0, 0.0,
                480, 320, 0.0,
                0, 1, 
                1, 0)
  end
end

function DrawBackgroundPartial(alpha, name)
  if GetPlatformIsiPad() then
    Ui_DrawQuad(name, alpha,
				-30, -20, 0.0,
                510, 340, 0.0,
                0,      1,
                0.9375, 0.375)
  else
	Ui_DrawQuad(name, alpha,
                  0,   0, 0.0,
                480, 320, 0.0,
                0,      1,
                0.9375, 0.375)
  end
end

LOADSTRINGS = {
"of death",
"human denial",
"completes the",
"the relic",
"immortality",
"the spring of",
"eternal life",
"the fountain of",
"and moon",
"the stars",
"who lofted",
"the ancients",
"profound art of",
"and life from",
"immortality",
"artifact of",
"the relic",
}

local THE_SEED = GetTime()

function DrawLoading()
  DrawBackgroundPartial(1.0, ChooseImage("background_partial", "background_partial_x2"))
--Ui_DrawStringCentered("loading", 240, 300, 50, 0.15)
  for i = 6, 1, -1 do
    local frame = ((GetGameStateFrames() + THE_SEED + i) % #LOADSTRINGS) + 1
    Ui_DrawStringCentered(LOADSTRINGS[frame], 400 - 50 * i, 320 - 10 * i, 20 + i * 10, 1.0 / i)
  end
end