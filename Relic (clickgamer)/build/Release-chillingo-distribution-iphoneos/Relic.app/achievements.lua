ACHIEVEMENTS = 
{
 	Level1          = { "1400957733", "Pandora's Box", "Awakened in the dungeon, seeking The Relic" },         -- x
 	Level2          = { "1400904821", "The Journey Ahead", "Completed level 1" },                              -- x
 	OpenDoor        = { "1400916976", "Break On Through", "Found a key, opened a door" },                      -- x
 	Kill25          = { "1400929473", "It's a Start", "25 creatures killed" },                                 -- x
  Ring            = { "1400973400", "The Immortal Ring", "Found the Amaranth Ring" },                        -- x
  DestroyedGoS    = { "1400905882", "Degenerator of Souls", "Destroyed the first Generator of Souls" },      -- x
 	PurchaseUpgrade = { "1400962552", "Shopping", "Visited an upgrade store for the first time" },             -- x
  MagicBolt       = { "1400921734", "Magic Bolt", "Acquired the Magic Bolt spell" },                         -- x
  x4              =	{ "1401056158", "Quadruple", "Achieved an attack multiplier x4" },
  Kill100         = { "1400934453", "The Executioner", "100 creatures killed" },                             -- x
  DivineTouch     = { "1400957459", "Touch of the Divine", "Acquired the Divine Touch spell" },              -- x
  Immolate        = { "1401017055", "The Firestarter", "Acquired the Immolate spell" },                      -- x
  Level7          = { "1401015440", "On the Road", "Completed level 6" },                                    -- x
  Shield          = { "1400989174", "Magic Shield", "Acquired the Magic Shield spell" },                     -- x
  Kill500         = { "1400973274", "The Master Slayer", "500 creatures killed" },                           -- x
  x5              = { "1400989288", "Quintuple", "Achieved an attack multiplier of x5" },
  Arachnarex      = { "1401014053", "The Exterminator", "Defeated the king of spiders, Arachnarex" },        -- x
  AtomicTouch     = { "1400908844", "Destroyer of Worlds", "Acquired the Atomic Touch spell" },              -- x
  Kill1000        = { "1400978227", "The Annihilator", "1,000 creatures killed" },                           -- x
  Quinotaur       = { "1401005512", "The Quinotaur", "Defeated the Quinotaur, the ruler of Amaranth" },      -- x
  x6              = { "1400942691", "Overdrive", "Achieved an attack multiplier of x6" },
  MaxMana         = { "1400994171", "Sorcerer", "The maximum mana possible" },                               -- x
  MaxHealth       = { "1400979271", "To Health", "The maximum health possible" },                            -- x
  Relic           = { "1401027182", "The Relic", "Defeated The Relic" },                                     -- x
  Kills5000       = { "1400969318", "Relentless", "5,000 creatures killed" },                                -- x
  Kills10000      = { "1401001031", "Is This Even Possible?", "10,000 creatures killed" },                   -- x
 	Credits         = { "1400958761", "Accredited", "Watched the credits" },                                   -- x
}

ACHIEVEMENTS_SHOWN = {}

LEADERBOARDS = 
{
  Kills = { "1401389775", "Kills", "Enemies Killed in The Relic" },
  Gold  = { "1401363978", "Gold",  "Gold Collected in The Relic" },
}

ACHIEVEMENT_LEADERBOARD_QUEUE = {}

function SubmitAchievementsQueue()
  for i = 1,#ACHIEVEMENT_LEADERBOARD_QUEUE do
    local call = ACHIEVEMENT_LEADERBOARD_QUEUE[i]
	if #call == 3 then
	  call[1](call[2], call[3])
	end
  end
  ACHIEVEMENT_LEADERBOARD_QUEUE = {}
end

function DirectShowAchievement(name)
  local achievement = ACHIEVEMENTS[name]
  if achievement == nil or ACHIEVEMENTS_SHOWN[name] == true then
    return
  end
  ACHIEVEMENTS_SHOWN[name] = true
  PostAchievement(achievement[1], achievement[2] .. ': ' .. achievement[3])  
  --table.insert(ACHIEVEMENT_LEADERBOARD_QUEUE, { PostAchievement, achievement[1], achievement[2] .. ': ' .. achievement[3]})
end

function ShowAchievement(name)
  local achievement = ACHIEVEMENTS[name]
  if achievement == nil or ACHIEVEMENTS_SHOWN[name] == true then
    return
  end
  ACHIEVEMENTS_SHOWN[name] = true
  --PostAchievement(achievement[1], achievement[2] .. ': ' .. achievement[3])  
  table.insert(ACHIEVEMENT_LEADERBOARD_QUEUE, { PostAchievement, achievement[1], achievement[2] .. ': ' .. achievement[3]})
end

function SubmitLeaderboard(name, value)
  local leaderboard = LEADERBOARDS[name]
  if leaderboard == nil then
    return 
  end
  --PostLeaderboard(leaderboard[1], value)
  table.insert(ACHIEVEMENT_LEADERBOARD_QUEUE, { PostLeaderboard, leaderboard[1], value})
end

ACHIEVEMENT_TYPES = { { name="Kills", interval=25 }, { name="Gold", interval=100 } }
UNREGISTERED = {}
LAST_VALUES = {}
UPDATE_ACHIEVEMENTS_TIME     = 0
UPDATE_ACHIEVEMENTS_INTERVAL = 5000

function SetAchievementValue(name, value)
  UNREGISTERED[name] = value
end

function AchieveValue(value, achieveInterval, leaderboardNameAndAchievementPrefix)
  local prefix = leaderboardNameAndAchievementPrefix
  local last   = LAST_VALUES[prefix]
  if last == nil or value > last then
    local val    = value - (value % achieveInterval)
    local key    = prefix .. val
    if ACHIEVEMENTS_SHOWN[key] ~= true then
      ACHIEVEMENTS_SHOWN[key] = true
      print ('Achieved: ' .. key)
      ShowAchievement(prefix .. key)
    end
    if value ~= 0 then
      print ('Leaderboard: ' .. value)
      SubmitLeaderboard(prefix, value)
    end
    LAST_VALUES[prefix] = value
  end
end

function UpdateAchievement(name, interval)
  if UNREGISTERED[name] ~= nil then 
    AchieveValue(UNREGISTERED[name], interval, name)
    UNREGISTERED[name] = nil
  end
end

function ForceUpdateAchievements()
  for i, v in ipairs(ACHIEVEMENT_TYPES) do
    UpdateAchievement(v.name, v.interval)
  end 
end

function UpdateAchievements()
  local tm = GetTime()
  if tm - UPDATE_ACHIEVEMENTS_TIME > UPDATE_ACHIEVEMENTS_INTERVAL then
    ForceUpdateAchievements()
    UPDATE_ACHIEVEMENTS_TIME = tm
  end 
end