print "Lua:heal.lua"

function Heal(c, hp)
  c:SetHitPoints(c:GetHitPoints() + hp, c)
end

function Heal_ById(cId, hp)
  Heal(GetCharacterById(cId), hp)
end

function MultiplayerHeal(c, hp)
  Heal(c, hp)
  Heal_ById(c:GetId(), hp)
end
