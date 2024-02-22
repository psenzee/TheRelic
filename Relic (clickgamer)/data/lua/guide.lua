print "lua:guide.lua"

function GiveRingQuestItems()
  AddToInventory(GetPlayer(), "RingQuest", 1)
  AddToInventory(GetPlayer(), "KeyRed", 1)
end

function GiveGeneratorQuestItems()
  AddToInventory(GetPlayer(), "Gen1Quest", 1)
  AddToInventory(GetPlayer(), "KeyBlue", 1)
end

function GiveAscendStairsItems()
  AddToInventory(GetPlayer(), "KeyWhite", 1)
end

DO_GUIDE_CLOSE = false

function GuideDialog()
  local level = GetLevelNumber()
  if level == 3 then
    if GetInventoryItemCount(GetPlayer(), "Ring") < 1 then
      if GetInventoryItemCount(GetPlayer(), "RingQuest") < 1 then
        StartSequence(GET_RING_SEQUENCE)
        GiveRingQuestItems()
        DO_GUIDE_CLOSE = false
      else
        MessageRise("@20\\#ffffffFind the", 0)
        MessageRise("@20\\#80ff80Amaranth Ring\\#ffffff", 0)
        DO_GUIDE_CLOSE = false
      end
    elseif GetInventoryItemCount(GetPlayer(), "KeyWhite") < 1 then
      StartSequence(KEY_ASCEND_STAIRS_SEQUENCE)
      GiveAscendStairsItems()
      DO_GUIDE_CLOSE = true
    else
      MessageRise("@20\\#ffffffAscend the stairs\\#ffffff", 0)    
      DO_GUIDE_CLOSE = false
    end    
  elseif level == 4 then
    if GetInventoryItemCount(GetPlayer(), "Gen1Quest") < 1 then
      StartSequence(GENERATOR_QUEST_SEQUENCE)
      GiveGeneratorQuestItems()
      DO_GUIDE_CLOSE = true
    elseif LOCAL_GAME_STATE.DestroyedCompleteGenerator then
      StartSequence(GENERATOR_DESTROYED_SEQUENCE)
      DO_GUIDE_CLOSE = true
    else
      MessageRise("@20\\#ffffffDestroy the", 0)      
      MessageRise("@20\\#80ff80Generator of Souls\\#ffffff", 0)    
      DO_GUIDE_CLOSE = false
    end
  elseif level == 5 then
    StartSequence(ORACLE_SEQUENCE)
    DO_GUIDE_CLOSE = true
  elseif level == 6 then
    StartSequence(GET_RING_SEQUENCE)
    DO_GUIDE_CLOSE = true
  elseif level == 7 then
    StartSequence(ARACHNAREX_SEQUENCE)
    DO_GUIDE_CLOSE = true
  elseif level == 8 then
  elseif level == 9 then
    MessageRise("@20\\#ffffffAscend to defeat", 0)
    MessageRise("@20the \\#80ff80Quinotaur\\#ffffff", 0)
    DO_GUIDE_CLOSE = false
  elseif level == 10 then
    StartSequence(QUINOTAUR_SEQUENCE_1)
    DO_GUIDE_CLOSE = true
  elseif level > 10 and level < 12 then
    MessageRise("@20\\#ffffffFind \\#ff8080The Relic\\#ffffff", 0)
    DO_GUIDE_CLOSE = false
  elseif level == 12 then
    MessageRise("@20\\#ffffffDestroy \\#ff8080The Relic\\#ffffff", 0)
    DO_GUIDE_CLOSE = false
  end
end

function Guide(c)
  c:SetRotation(3.1415926)
end

function GuideListener(info, data)
--StartDialog()
  GuideDialog()
end

function ShouldClearGuide(c)
  return DO_GUIDE_CLOSE
end

function CreateGuide_Character(c, type, hp, speed, x, y)

   SetWhiteHaloEffect(c)

   c:SetFixed(true)
   c:SetStatic(true)
   c:AddBehavior(NewAnimationBehavior())

   SetAttributes(c, 10000000.0, 0)
   c:SetScale(1.1 * characterScale)

   c:SetRadius(characterRadius)   
   c:data().isGuide = true
   c:data().selectEvent = "Select_Guide"   
   c:data().selectClose = true
   c:data().selectClear = ShouldClearGuide
   c:data().selector = InteractSelector
   
   DO_GUIDE_CLOSE = false -- reset the close after we create guides
end

function CreateGuide(type, hp, speed, x, y)
   local c = NewCharacter(-1, "Guide", type, x, y, 0.0)
   CreateGuide_Character(c, type, hp, speed, x, y)
end

