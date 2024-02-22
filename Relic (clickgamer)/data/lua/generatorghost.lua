print "lua:generatorghost.lua"

generatorGhostRadius = characterRadius
generatorGhostHeight = -100.0

function GeneratorGhost(c)
 SetTarget(c)
end

CREATED_GENERATORS = {}
DESTROYED_GENERATORS = {}

function MarkDestroyed(c)
  table.insert(DESTROYED_GENERATORS, c)
  for i, value in ipairs(CREATED_GENERATORS) do 
    if value == c then
      LOCAL_GAME_STATE.DestroyedGeneratorGhosts[i] = true
    end
  end
end

function GeneratorDestroyedMessage(level)
  if level == 12 then
    ShowAchievement('Relic')
    CompleteEntireGame()
  elseif GetPlayer() ~= nil then
    --RetrieveFromInventory(GetPlayer(), "Gen1Quest", 1)
    AddToInventory(GetPlayer(), "KeyWhite", 1)
    ShowDestroyGen1QuestComplete()
    ClearDestroyGen1QuestDialog()
  end
end

function AllDestroyed()
  local id = "Generator" .. GetLevelNumber() .. "Destroyed"
  FireEvent(id, "")
  local x, y, z = GetPlayer():GetPosition()
  Audio_PlayAt("Explode", x, y, z, 6.0)
  for i, value in ipairs(DESTROYED_GENERATORS) do
    value:SetVisible(false)
    SetDestroyEffect1(value)
    LOCAL_GAME_STATE.DestroyedCompleteGenerator = true
  end
  DoFlash(3000)
  Audio_PlayAt("#vibrate", x, y, z, 6.0)
  DeferByTimeMs(3000, "GeneratorDestroyedMessage", GetLevelNumber())
  ShowAchievement('DestroyedGoS')
end

function DestroyGeneratorGhostSilent(c)
  c:AddSignalHandler(SIGNAL_DIED, "GeneratorGhostDestroyedSilent")
  c:SetHitPoints(c, 0, c)
end

function GeneratorGhostDestroyedSilent(c)
   c:SetFixed(false)
   c:SetMovable(true)
   c:SetCollidable(false)
   MarkDestroyed(c)
   if LOCAL_GAME_STATE.ActiveGeneratorGhosts ~= nil then
     LOCAL_GAME_STATE.ActiveGeneratorGhosts = LOCAL_GAME_STATE.ActiveGeneratorGhosts - 1
     if LOCAL_GAME_STATE.ActiveGeneratorGhosts <= 0 then
       AllDestroyed()
     end
   end
end

function GeneratorGhostDestroyed(c)
   GeneratorGhostDestroyedSilent(c)
   SetDestroyEffect1(c)
   local x, y, z = c:GetPosition()    
   DoFlash(1000)
   Audio_PlayAt("Electricity", x, y, z, 2.0)
   Audio_PlayAt("#vibrate", x, y, z, 1.0)
   --Audio_PlayAt("Hiss1", x, y, z, 1.0)
   --Audio_PlayAt("Hiss2", x, y, z, 1.0)
  c:CompleteEffects()   
end

function GeneratorGhostHit(c)
  local xp, yp, zp = c:GetPosition()
  Audio_PlayAt("SkeletonHit", xp, yp, zp, 0.25)
  --Audio_PlayAt("Electricity", xp, yp, zp, 1.0)  
  SetHitGeneratorEffect(c)
end

function GeneratorGhostSelector(c)
  c:SetSelector("red2", 16, 20.0, 50.0, 0.025)
end

function CreateGeneratorGhost_Character(c, type, hp, speed, x, y, seed)

   c:AddBehavior(NewAnimationBehavior())
   c:AddBehavior(NewKillableBehavior())
   c:AddBehavior(NewDieCollapseBehavior())

   SetAttributes(c, RandomBetweenSeeded(seed, hp / 2, hp), 0)
   c:SetScale(1.0 * characterScale)
   c:SetFixed(true)
   c:SetCollidable(true)
   c:SetMovable(false)
   c:SetKnockBack(0.0)
   c:SetRadius(generatorGhostRadius)
   c:SetApproachSpeed(0)
   c:AddSignalHandler(SIGNAL_DIED,         "GeneratorGhostDestroyed")
   c:AddSignalHandler(SIGNAL_RECEIVED_HIT, "GeneratorGhostHit")
   
   c:data().selector = GeneratorGhostSelector
   
   table.insert(CREATED_GENERATORS, c)
end

function CreateGeneratorGhost(type, hp, speed, x, y, seed)
   local c = NewCharacter(-1, "GeneratorGhost", type, x, y, 0.0)
   CreateGeneratorGhost_Character(c, type, hp, speed, x, y, seed)
end