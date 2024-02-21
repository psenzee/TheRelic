print "Lua:uidraw.lua"

MESSAGE_RISE = true

function MessageRise(text, delay)
  if MESSAGE_RISE then
    SendRisingMessage(text, delay)
  end
end

function DrawBackground(alpha, name)
  local x, y, w, h = GetUiBounds()
  Ui_DrawQuad(name, alpha, x, y, 0.0, w, h, 0.0, 0, 1, 1, 0)
end

function DrawBackgroundPartial(alpha, name)
  local x, y, w, h = GetUiBounds()
  Ui_DrawQuad(name, alpha, x, y, 0.0, w, h, 0.0, 0, 1, 0.9375, 0.375)
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
  local x, y, w, h = GetUiBounds()
  DrawBackgroundPartial(1.0, "background_partial_x2")
  for i = 6, 1, -1 do
    local frame = ((GetGameStateFrames() + THE_SEED + i) % #LOADSTRINGS) + 1
    Ui_DrawStringCentered(LOADSTRINGS[frame], w * 0.8 - 50 * i, h - 10 * i, 20 + i * 10, 0.5 / i)
  end
end
