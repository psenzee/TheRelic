print("Lua:rpg.lua")

function GetExperienceYield(c)
  local data = c:data()
  return data.hitPointsMax * data.armor * GLOBAL_EXPERIENCE_SCALE
end

-- // if you don't seek out and kill all the enemies, you're not going to advance fast enough and will have to go back
-- // have to multiply the number of enemies by the number of players, so that there will be enough experience to go around (or not quite enough)
-- dj_level =~= player_level
-- // player should gain a constant number of levels per dj_level? Or a decreasing number?
-- enemies_count[LevelX] = X % 3 : { few, moderate, many };
-- // a hidden variable here is each level's potential total xp gain
-- LevelX.total_xp = LevelX-1.total_xp * xp_factor; // xp_factor = 1.5f?
-- average_xp_per_enemy[LevelX] = LevelX.total_xp / enemies_count[LevelX]
-- // meaning : LevelX.total_xp = enemies_count[LevelX] * average_xp_per_enemy[LevelX] {of course}
