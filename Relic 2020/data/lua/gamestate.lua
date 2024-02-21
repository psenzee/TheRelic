GameTemplateState = 
{
  Version                  = 1.0,
  QuestionsSelected        = {},
  QuestionsAnswered        = {},
  AnswersRead              = {},
  Journal                  = {},
  LevelsComplete           = { false, false, false, false,
                               false, false, false, false,
                               false, false, false, false,
                               false, false, false, false },
  LevelsVisited            = { false, false, false, false,
                               false, false, false, false,
                               false, false, false, false,
                               false, false, false, false },                               
  ActiveGeneratorGhosts    = 0,
  DestroyedGeneratorGhosts = { false, false, false, false, false, false,
                               false, false, false, false, false, false,
                               false, false, false, false, false, false,
                               false, false, false, false, false, false,
                               false, false, false, false, false, false,
                               false, false, false, false, false, false,
                               false, false, false, false, false, false,
                               false, false, false, false, false, false, },
  DestroyedCompleteGenerator = false,
  TurnEnemies                = false,
  GameCompleted              = false,
}

LOCAL_GAME_STATE = deepcopy(GameTemplateState)

function StartPlayerGameStateLevel(level)
  LOCAL_GAME_STATE.ActiveGeneratorGhosts = 6
  if level == 12 then 
    -- the relic itself consist of four generators
    LOCAL_GAME_STATE.ActiveGeneratorGhosts = LOCAL_GAME_STATE.ActiveGeneratorGhosts * 4 
  end
  LOCAL_GAME_STATE.DestroyedGeneratorGhosts = { false, false, false, false, false, false,
                                                false, false, false, false, false, false,
                                                false, false, false, false, false, false,
                                                false, false, false, false, false, false,
                                                false, false, false, false, false, false,
                                                false, false, false, false, false, false,
                                                false, false, false, false, false, false,
                                                false, false, false, false, false, false, }
end

function StartPlayerGameState()
  LOCAL_GAME_STATE                       = deepcopy(GameTemplateState)
  LOCAL_GAME_STATE.DIALOG_TEXT           = deepcopy(DIALOG_TEXT)
  LOCAL_GAME_STATE.DIALOG_OPTION_TEXT    = deepcopy(DIALOG_OPTION_TEXT)
  LOCAL_GAME_STATE.DIALOGS               = deepcopy(DIALOGS)
  LOCAL_GAME_STATE.DIALOG_ANSWERS        = deepcopy(DIALOG_ANSWERS)
  LOCAL_GAME_STATE.START_DIALOG_ID       = "L1_A_IWait"
  LOCAL_GAME_STATE.CURRENT_DIALOG_ID     = LOCAL_GAME_STATE.START_DIALOG_ID
  LOCAL_GAME_STATE.DIALOG_STACK          = {}
  LOCAL_GAME_STATE.ActiveGeneratorGhosts = 6
end

function ClearAmaranthRingQuestDialog()
  LOCAL_GAME_STATE.DIALOGS.L1_A_HowToDestroyIt = { "L1_Q_HaveRingWhatNext" }
end

function ClearDestroyGen1QuestDialog()
  LOCAL_GAME_STATE.START_DIALOG_ID = "L1_A_DestroyedGenerator"
  LOCAL_GAME_STATE.CURRENT_DIALOG_ID  = LOCAL_GAME_STATE.START_DIALOG_ID
  LOCAL_GAME_STATE.DIALOG_STACK = {}  
end  