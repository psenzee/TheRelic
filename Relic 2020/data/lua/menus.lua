print "Lua:menus.lua"

WIDTH  = _G.UI_RIGHT
HEIGHT = _G.UI_BOTTOM

MAIN_MENU = nil                                                                                                                          
MULTIPLAYER_MENU = nil
START_NEW_MENU = nil
SAVE_EXIT_MENU = nil
LEVEL_EXIT_WAIT_MENU = nil
REALLY_SAVE_MENU = nil
REALLY_QUIT_MENU = nil
NEW_LOAD_GAME_MENU = nil
EXTRAS_MENU = nil
IN_GAME_MENU = nil
GO_TO_MAP_MENU = nil
CHARACTER_TIP = nil
MAGIC_TIP = nil
HEALTH_TIP = nil
GOLD_TIP = nil
REDKEY_TIP = nil
BLUEKEY_TIP = nil
WHITEKEY_TIP = nil
TURN_TIP = nil
MENU_TIP = nil
SKIP_MENU = nil

LOAD_GAME_BUTTON = nil

CREDITS_SCREEN = nil
TURNOFFWIFI_SCREEN = nil
TURNOFFBLUETOOTH_SCREEN = nil
CONNECTFAILED_SCREEN = nil

MAP_PAGE = nil
ATTRIBUTES_PAGE = nil
STATS_PAGE = nil
QUESTS_PAGE = nil
SETTINGS_PAGE = nil
MULTIPLAYER_PAGE = nil
CREDITS_PAGE = nil
DEBUG_PAGE = nil
BACKGROUND_MENU_IMAGE = nil
BACKGROUND_MAIN_MENU_IMAGE = nil

TITLE_SEQUENCE_STARTED = false

function StartTitleSequence()
  if TITLE_SEQUENCE_STARTED == false then
    StartSequence(TITLE_SEQUENCE)
    TITLE_SEQUENCE_STARTED = true
	if GetPlatformIsiPad() then
	  ActivateCrystaliPad()
    end
  end
end

function EndTitleSequence()
  if GetPlatformIsiPad() then
    DeactivateCrystaliPad()
  end
  PlayMusicTheme("Relic")
  TITLE_SEQUENCE_STARTED = false
  EndSequence()
  UnloadSequenceTextures()
end

---

TITLE_START = 40

function ResetCreditScroll()
  CREDIT_SCROLL = 180
end

MENUS_WIDTH = 1500
MENUS_TOP   = 200

function _SetVisibleForce(menu, visible)
if UiControl_IsVisible(menu) ~= visible then
    UiControl_SetVisible(menu, visible)
  end
end

function _SetVisible(menu, visible)
  if menu ~= nil then
    _SetVisibleForce(menu, visible)
  end
end

function StartUi()
  local x, y, w, h = GetUiBounds()
  if CANVAS == nil then
    CANVAS = Ui_CreateMenu("Canvas", "", "", x, y, w, h, 1.0, 22.5)
    Ui_SetRootControl(CANVAS)
  end  
end

function StartMenus()
  StartUi()
  CreateMenus()
  HideMenus()
end
                    
function EnableContinueSavedGame()
  local saveexist = DoesAnySavedGameExist()
  UiControl_SetActive(LOAD_GAME_BUTTON, saveexist)
  local text = "Continue Saved Game"
  if not saveexist then
    text = "\\#808080" .. text
  end
  UiControl_SetText(LOAD_GAME_BUTTON, text)
end
                    
function ShowMainMenu()
  PlayMusicTheme("Relic")
  --PlayMusicTheme("AndyLivingston_Heavy")
  HideMenus()
  SetDarkness(0.0)
  IS_STARTING_CLIENT = false
  UiControl_SetActive(MAIN_MENU, true) 
  _SetVisible(MAIN_MENU, true)
  EnableContinueSavedGame()
  StartTitleSequence()
  _SetVisible(BACKGROUND_MAIN_MENU_IMAGE, false)
  SetGameStateId(GAMESTATE_MENU)
  if ShouldCrystalActivate() then
    StartCrystal()
  end
end

function CloseMenus()
  HideMenus()
  _SetVisible(BACKGROUND_MAIN_MENU_IMAGE, false)  
  UnloadTexture("menuback_partial")    
end

function ShowExtrasMenu()
  HideMenus()
  _SetVisible(EXTRAS_MENU, true)  
  StartTitleSequence()
  --_SetVisible(BACKGROUND_MAIN_MENU_IMAGE, true)
  _SetVisible(BACKGROUND_MAIN_MENU_IMAGE, false)
end

function ShowCredits()
  --local theme = "AndyLivingston_Heavy"
  EndTitleSequence()
  --if (math.random(1, 2) < 1.5) then theme = "AndyLivingston_IndigoWinter" end
  theme = "AndyLivingston_IndigoWinter"
  PlayMusicTheme(theme)
  ResetCreditScroll()
  DoFlash(2000)
  HideMenus()
  --_SetVisible(CREDITS_DISMISS_BG, false)  
  _SetVisible(CREDITS_SCREEN, true)
  _SetVisible(BACKGROUND_MAIN_MENU_IMAGE, false)
  SetGameStateId(GAMESTATE_MENU)
end

function ShowLoadGameMenu()
  HideMenus()
  _SetVisible(NEW_LOAD_GAME_MENU, true)
  StartTitleSequence()
  --_SetVisible(BACKGROUND_MAIN_MENU_IMAGE, true)
  _SetVisible(BACKGROUND_MAIN_MENU_IMAGE, false)
end

function ShowMultiplayerMenu()
  HideMenus()
  _SetVisible(MULTIPLAYER_MENU, true)
  StartTitleSequence()
  --_SetVisible(BACKGROUND_MAIN_MENU_IMAGE, true)
  _SetVisible(BACKGROUND_MAIN_MENU_IMAGE, false)
end

function ShowTurnOffWiFi()
  HideMenus()
  _SetVisible(TURNOFFWIFI_SCREEN, true)
  _SetVisible(BACKGROUND_MAIN_MENU_IMAGE, false)
  --UiControl_SetAlpha(BACKGROUND_MAIN_MENU_IMAGE, 0.5)
end

function ShowConnectFailed()
  HideMenus()
  SetGameStateId(GAMESTATE_MENU)
  _SetVisible(CONNECTFAILED_SCREEN, true)
  _SetVisible(BACKGROUND_MAIN_MENU_IMAGE, false)
  --UiControl_SetAlpha(BACKGROUND_MAIN_MENU_IMAGE, 0.5)
end

function ShowTurnOffBluetooth()
  HideMenus()
  _SetVisible(TURNOFFBLUETOOTH_SCREEN, true)
  _SetVisible(BACKGROUND_MAIN_MENU_IMAGE, false)
  --UiControl_SetAlpha(BACKGROUND_MAIN_MENU_IMAGE, 0.5)  
end

function ShowStartNewMenu()
  HideMenus()
  _SetVisible(START_NEW_MENU, true)
  StartTitleSequence()
  --_SetVisible(BACKGROUND_MAIN_MENU_IMAGE, true)
  _SetVisible(BACKGROUND_MAIN_MENU_IMAGE, false)
end

function ShowSaveExitMenu()
  HideMenus()
  SetPaused(true)
  PROCESS_BUTTONS = false
  SetSuppressHud(true)
  SetDarkness(0.5)
  NO_SAVE_EXIT_RETURN = ShowSaveExitMenu
  _SetVisible(SAVE_EXIT_MENU, true)
--_SetVisible(BACKGROUND_MAIN_MENU_IMAGE, true)
end

function ShowLevelExitWaitMenu()
  HideMenus()
  SetPaused(true)
  PROCESS_BUTTONS = false
  SetSuppressHud(true)
  SetDarkness(0.5)
  _SetVisible(LEVEL_EXIT_WAIT_MENU, true)
  SetThisWaitingForOthers(true)
end

function ShowNoDescendMenu()
  HideMenus()
  SetPaused(true)
  PROCESS_BUTTONS = false
  SetSuppressHud(true)
  SetDarkness(0.5)
  _SetVisible(NO_DESCEND_MENU, true)
end

function ShowGoToMapMenu()
  HideMenus()
  SetPaused(true)
  PROCESS_BUTTONS = false
  SetSuppressHud(true)
  SetDarkness(0.5)
  NO_SAVE_EXIT_RETURN = ShowGoToMapMenu
  _SetVisible(GO_TO_MAP_MENU, true)
end

function ShowSkipMenu()
  HideMenus()
  --SetPaused(true)
  --PROCESS_BUTTONS = false
  SetSuppressHud(true)
  --SetDarkness(0.5)
  _SetVisible(Skip_MENU, true)
end

function ShowTipType(tip)
  if IsPaused() then
    return false
  end
  HideMenus()
  SetPaused(true)
  PROCESS_BUTTONS = false
  SetSuppressHud(true)
  SetDarkness(0.5)
  LockInputForTimeMs(1000)
  _SetVisible(tip, true)
  return true
end

function ShowCharacterTip()
  return ShowTipType(CHARACTER_TIP)
end

function ShowTurnTip()
  return ShowTipType(TURN_TIP)
end

function ShowMagicTip()
  return ShowTipType(MAGIC_TIP)
end

function ShowHealthTip()
  return ShowTipType(HEALTH_TIP)
end

function ShowManaTip()
  return ShowTipType(MANA_TIP)
end

function ShowGoldTip()
  return ShowTipType(GOLD_TIP)
end

function ShowRedKeyTip()
  return ShowTipType(REDKEY_TIP)
end

function ShowBlueKeyTip()
  return ShowTipType(BLUEKEY_TIP)
end

function ShowWhiteKeyTip()
  return ShowTipType(WHITEKEY_TIP)
end

function ShowSoulTip()
  return ShowTipType(SOUL_TIP)
end

function ShowReallySaveMenu()
  HideMenus()
  SetPaused(true)
  PROCESS_BUTTONS = false
  SetSuppressHud(true)
  SetDarkness(0.5)
  _SetVisible(REALLY_SAVE_MENU, true)
--_SetVisible(BACKGROUND_MAIN_MENU_IMAGE, true)
end

function ShowReallyQuitMenu()
  HideMenus()
  SetPaused(true)
  PROCESS_BUTTONS = false
  SetSuppressHud(true)
  SetDarkness(0.5)
  _SetVisible(REALLY_QUIT_MENU, true)
--_SetVisible(BACKGROUND_MAIN_MENU_IMAGE, true)
end

function ShowInGameMenuPage2()
  HideInGameMenu()
  ShowInGameElementsPage2()
  PROCESS_BUTTONS = false
end

function ShowInGameMenuPage1()
  HideInGameMenu()
  ShowInGameElements()
  PROCESS_BUTTONS = false  
end

function ShowInGameItemsPage()
  ShowInGameMenuPage1()
  UiInventoryShow()
end

function ShowInGameMapPage()
  ShowInGameMenuPage1()
  _SetVisible(MAP_PAGE, true)
  --ShowIndicatorForce('GoldIndicator')
  --ShowIndicatorForce('KillsIndicator')
  --local instance = INDICATORS['GoldIndicator'].instance
  --UiControl_SetVisible(instance, true)
  --instance = INDICATORS['KillsIndicator'].instance
  --UiControl_SetVisible(instance, true)
end

function ShowInGameAttributesPage()
  ShowInGameMenuPage1()
  UpdateAttributes(GetPlayer())
  _SetVisible(ATTRIBUTES_PAGE, true)
end

function ShowInGameStatsPage()
  ShowInGameMenuPage1()
  _SetVisible(STATS_PAGE, true)
end

function ShowInGameQuestsPage()
  ShowInGameMenuPage1()
  _SetVisible(QUESTS_PAGE, true)
end

function ShowInGameSettingsPage()
  ShowInGameMenuPage2()
  _SetVisible(SETTINGS_PAGE, true)
end

function ShowInGameMultiplayerPage()
  ShowInGameMenuPage2()
  _SetVisible(MULTIPLAYER_PAGE, true)
end

function ShowInGameCreditsPage()
  ShowInGameMenuPage2()
  _SetVisible(CREDITS_PAGE, true)
end

function ShowInGameDebugPage()
  ShowInGameMenuPage2()
  UpdateInGameDebugPage()
  _SetVisible(DEBUG_PAGE, true)
end

function HideMenus()
  SetDarkness(0.0)
  _SetVisible(MAIN_MENU, false)
  _SetVisible(MULTIPLAYER_MENU, false)
  _SetVisible(NEW_LOAD_PROFILE_MENU, false)
  _SetVisible(NEW_LOAD_GAME_MENU, false)
  _SetVisible(START_NEW_MENU, false)
  _SetVisible(SAVE_EXIT_MENU, false)
  HideUpgradeMenus()
  HideLoadSaveMenus()
  _SetVisible(REALLY_SAVE_MENU, false)
  _SetVisible(REALLY_QUIT_MENU, false)
  _SetVisible(NO_DESCEND_MENU, false)
  _SetVisible(EXTRAS_MENU, false)
  _SetVisible(GO_TO_MAP_MENU, false)
  _SetVisible(CHARACTER_TIP, false)
  _SetVisible(MAGIC_TIP, false)
  _SetVisible(HEALTH_TIP, false)
  _SetVisible(MANA_TIP, false)
  _SetVisible(GOLD_TIP, false)
  _SetVisible(SOUL_TIP, false)
  _SetVisible(REDKEY_TIP, false)
  _SetVisible(BLUEKEY_TIP, false)
  _SetVisible(WHITEKEY_TIP, false)
  _SetVisible(TURN_TIP, false)  
--_SetVisible(BACKGROUND_MAIN_MENU_IMAGE, false)
  _SetVisible(CREDITS_SCREEN, false)
  _SetVisible(TURNOFFWIFI_SCREEN, false)
  _SetVisible(TURNOFFBLUETOOTH_SCREEN, false)
  _SetVisible(CONNECTFAILED_SCREEN, false)
  _SetVisible(LEVEL_EXIT_WAIT_MENU, false)
  _SetVisible(CREDITS_DISMISS_BG, false)
  PROCESS_BUTTONS = true
end

function ExitMenus()
  HideMenus()
  if IsMapOn() then
    ToggleMap() -- here assuming that map is on
  end
end

function ResumeFromInGame()
  ExitMenus()
  SetPaused(false)
  SetSuppressHud(false)
  SetThisWaitingForOthers(false)
end

function UpLevel()
  ExitMenus()
  UpStairsListener("Enter", {})  
end

function DownLevel()
  ExitMenus()
  DownStairsListener("Enter", {})  
end

function SetZoom(zoom)
  BASE_DEPTH_SCALE = zoom
  ExitMenus()
end

function DoSaveGameUi()
  --ShowReallySaveMenu()
  ShowSaveListMenu()
end

function LinkListener(info, data)
  if info == 'SELECTED' then
    GoToLink(data.name)
  end
end

CONTINUE_TO_MULTIPLAYER_FUNCTION = nil

function CreateMenuType(type, atY)
  local x, y, w, h = GetUiBounds()
  local center = w * 0.5 + x
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  return Ui_CreateMenu(type, "border3", "..", center - halfWidth, atY, center + halfWidth, atY, 1.0, 22.5)
end

function ContinueToMultiplayer()
  if CONTINUE_TO_MULTIPLAYER_FUNCTION ~= nil then
    CONTINUE_TO_MULTIPLAYER_FUNCTION()
  else
    print("** ERROR, CONTINUE_TO_MULTIPLAYER_FUNCTION is nil **")
  end
end

function StartNewGameWiFi()
  CONTINUE_TO_MULTIPLAYER_FUNCTION = MainMenu_StartNewGameWiFi
  ShowTurnOffBluetooth()
end

function StartNewGameBluetooth()
  CONTINUE_TO_MULTIPLAYER_FUNCTION = MainMenu_StartNewGameBluetooth
  ShowTurnOffWiFi()
end

function LoadGameWiFi()
  CONTINUE_TO_MULTIPLAYER_FUNCTION = MainMenu_LoadGameWiFi
  ShowTurnOffBluetooth()
end

function LoadGameBluetooth()
  CONTINUE_TO_MULTIPLAYER_FUNCTION = MainMenu_LoadGameBluetooth
  ShowTurnOffWiFi()
end

function JoinGameWiFi()
  CONTINUE_TO_MULTIPLAYER_FUNCTION = MainMenu_JoinWiFiGame
  ShowTurnOffBluetooth()
end

function JoinGameBluetooth()
  CONTINUE_TO_MULTIPLAYER_FUNCTION = MainMenu_JoinBluetoothGame
  ShowTurnOffWiFi()
end

function UiSelectSound()
  Audio_PlayAt("SwordOnSword", 0.0, 0.0, 0.0, 1.0)
end

function UiFocusSound()
  Audio_PlayAt("Pickup", 0.0, 0.0, 0.0, 1.0)
end

function HideMenusRestoreHud()
  HideMenus()
  SetPaused(false)
  PROCESS_BUTTONS = true
  SetSuppressHud(false)
  SetDarkness(0.0)  
end

LAST_FOCUSED = nil

NO_SAVE_EXIT_RETURN = nil

function MenuListener(info, data)
  if info == 'FOCUSED' and LAST_FOCUSED ~= data.name then
    UiFocusSound()  
    LAST_FOCUSED = data.name
    if data.name == 'FocusBack' then HideMenusRestoreHud() end
  end
  if info == 'SELECTED' then
    UiSelectSound()  
    if     data.name == 'Resume'         then if GetGameStateId() == GAMESTATE_MENU then ShowMainMenu() else ResumeFromInGame() end
    elseif data.name == 'ExitGame'       then
      if IsMapOn() then
        ToggleMap()
      end
      ShowReallyQuitMenu()
    elseif data.name == 'EndGame'        then EndGameReturnToMenu()
    elseif data.name == 'SaveGame'       then DoSaveGameUi()
    elseif data.name == 'NoSaveOrQuitGame' then 
      if NO_SAVE_EXIT_RETURN == nil then    
        ShowSaveExitMenu()
      else
        NO_SAVE_EXIT_RETURN()
      end
    elseif data.name == 'GoToMap'        then
      HideMenusRestoreHud()
      if not ToggleMap() then
        ToggleMap()
      end
    elseif data.name == 'Back'           then HideMenusRestoreHud();
    elseif data.name == 'YesSaveGame'    then ResumeFromInGame(); SavePrimaryPlayer(SAVE_FILENAME)
    elseif data.name == 'MainMenu'       then ShowMainMenu()
    elseif data.name == 'Credits'        then ShowCredits()
    elseif data.name == 'EndCredits'     then EndCredits()
--  elseif data.name == 'MainMenu_StartNewGame' then ShowStartNewMenu()
    elseif data.name == 'LoadComplete_NextLevel' then HideMenusRestoreHud(); AscendNextLevel()
    elseif data.name == 'MainMenu_StartNewGame' then EndTitleSequence(); MainMenu_StartNewGameSinglePlayer()
    elseif data.name == 'CrystalAchievements' then ShowCrystalAchievements()
    elseif data.name == 'CrystalLeaderboards' then ShowCrystalLeaderboards()
    elseif data.name == 'StartNewMenu_SinglePlayer' then EndTitleSequence(); MainMenu_StartNewGameSinglePlayer()
    elseif data.name == 'StartNewMenu_UseWiFi'      then StartNewGameWiFi() --MainMenu_StartNewGameWiFi()    
    elseif data.name == 'StartNewMenu_UseBluetooth' then StartNewGameBluetooth() --MainMenu_StartNewGameBluetooth()
    elseif data.name == 'LoadMenu_SinglePlayer'     then 
--    EndTitleSequence(); 
      MainMenu_LoadGameSinglePlayer() 
    elseif data.name == 'LoadMenu_UseWiFi'          then LoadGameWiFi() --MainMenu_LoadGameWiFi()
    elseif data.name == 'LoadMenu_UseBluetooth'     then LoadGameBluetooth() --MainMenu_LoadGameBluetooth()
--  elseif data.name == 'MainMenu_LoadGame'     then ShowLoadGameMenu() 
    elseif data.name == 'MainMenu_LoadGame'     then 
--    EndTitleSequence(); 
      MainMenu_LoadGameSinglePlayer()
    elseif data.name == 'MainMenu_JoinGame'     then ShowMultiplayerMenu() 
    elseif data.name == 'MainMenu_Crystal'      then _G.DO_START_CRYSTAL = true	
    elseif data.name == 'MPMenu_UseWiFi'        then JoinGameWiFi() --MainMenu_JoinWiFiGame()
    elseif data.name == 'MPMenu_UseBluetooth'   then JoinGameBluetooth() --MainMenu_JoinBluetoothGame()
    elseif data.name == 'MainMenu_Extras'       then ShowExtrasMenu() 
    elseif data.name == 'MainMenu_Alert'        then SendMemoryWarning() 	
    elseif data.name == 'ContinueToMultiplayer' then EndTitleSequence(); ContinueToMultiplayer() 
    elseif data.name == 'StartBluetoothPicker' then StartBluetoothPicker() 
    elseif data.name == 'StartWiFiPicker' then StartWiFiPicker() 
    elseif data.name == 'StartBonjour'    then StartBonjour()
    elseif data.name == 'To_NextIGPage'   then ShowInGameSettingsPage() --ShowInGameMenuPage2()
    elseif data.name == 'To_PrevIGPage'   then ShowInGameMapPage() -- ShowInGameMenuPage1()
    elseif data.name == 'To_Items'        then ShowInGameMapPage()
    elseif data.name == 'To_Map'          then ShowInGameMapPage()
    elseif data.name == 'To_Attributes'   then ShowInGameAttributesPage()
    elseif data.name == 'To_Stats'        then ShowInGameStatsPage()
    elseif data.name == 'To_Quests'       then ShowInGameQuestsPage()
    elseif data.name == 'To_Settings'     then ShowInGameSettingsPage()
    elseif data.name == 'To_Multiplayer'  then ShowInGameMultiplayerPage()
    elseif data.name == 'To_Credits'      then ShowInGameCreditsPage()
    elseif data.name == 'To_$DEBUG$'      then ShowInGameDebugPage()
    elseif data.name == 'InviteFriend'    then SendEmail("Play The Relic", "Play the Relic")
    elseif data.name == 'SendScore'       then SendEmail("Check out my gold in The Relic", "I have 10000000 gold")	
    elseif data.name == 'UpLevel'         then UpLevel()
    elseif data.name == 'DownLevel'       then DownLevel()
    elseif data.name == 'Zoom0_25'        then SetZoom(0.25)
    elseif data.name == 'Zoom0_5'         then SetZoom(0.5)
    elseif data.name == 'Zoom0_75'        then SetZoom(0.75)
    elseif data.name == 'Zoom1_0'         then SetZoom(1.0)
    elseif data.name == 'Zoom1_25'        then SetZoom(1.25)
    elseif data.name == 'Zoom1_5'         then SetZoom(1.5)
    elseif data.name == 'Zoom2_0'         then SetZoom(2.0)
    elseif data.name == 'MakeRedKey'      then AddToPlayerInventoryMax("KeyRed",  1, 1)
    elseif data.name == 'MakeWhiteKey'    then AddToPlayerInventoryMax("KeyWhite", 1, 1)
    elseif data.name == 'MakeBlueKey'     then AddToPlayerInventoryMax("KeyBlue", 1, 1)
    elseif data.name == 'MakePortalSpell' then AddToPlayerInventory("Potion5", 1)
    elseif data.name == 'MakeTurnSpell'   then AddToPlayerInventory("Book2", 1)
    elseif data.name == 'GoToGenerator'   then
      print ("Go to generator")
      local count = GetNamedPointCount("GeneratorWest")
      if count > 0 then
        local x, y, z = GetNamedPoint("GeneratorWest", 0);
        ExitMenus()
        print("Placing portal at " .. tostring(x - 128) .. ", " .. tostring(y))
        PlacePortalAAt(x - 128, y - 256)
       end   
    elseif data.name == 'Add1000MaxHP'   then 
      local hp = GetPlayer():GetMaxHitPoints() + 1000
      GetPlayer():SetMaxHitPoints(hp)
      GetPlayer():SetHitPoints(hp, GetPlayer())
    elseif data.name == 'LoadGame'        then
      ExitMenus()
      DeferByTimeMs(500, "ShowLoadGameDialog", "Are you sure?\n\nYou will lose any unsaved\nprogress.", "Load Game")
    elseif HandleUpgradeEvent(data.name) then 
    elseif HandleSaveLoadEvent(data.name) then 
    end
  end
end

function AddMenuButton(parent, name, text, width)
  local button = Ui_CreateButton(name, "border3_3", text, 0, 0, width, 35, 1.0, 25)
  UiControl_SetMargin(button, 2, 2)
  UiControl_AddChild(parent, button)
  RegisterUiListener(name, "MenuListener", { name = name })
  return button
end

function AddImageMenuButton(parent, name, image, text, width, iconAtPercent, buttonVarName)
  local menu = Ui_CreateMenu(name .. "_Menu", "border3", "", 0, 0, width, 35, 1.0, 22.5)
  local button = Ui_CreateButton(name, "border3_3", text, 0, 0, width, 35, 1.0, 25)
  _G[buttonVarName] = button  
  local at = width * iconAtPercent
  local image = Ui_CreateImageButton(name .. "_Image", image, "", at, -2, at + 40, 40, 1.0, 25, 0, 1, 1, 0, 0)
  UiControl_SetMargin(button, 2, 2)
  UiControl_AddChild(menu, button)
  UiControl_AddChild(menu, image)
  RegisterUiListener(name, "MenuListener", { name = name })
  RegisterUiListener(name .. "_Image", "MenuListener", { name = name })
  UiControl_AddChild(parent, menu)
  return menu
end  

function AddTip(parent, name, text, width, height)
  local button = Ui_CreateButton(name, "blank", text, 0, 0, width, height, 1.0, 25)
  UiControl_SetMargin(button, 2, 2)
  UiControl_AddChild(parent, button)
  RegisterUiListener(name, "MenuListener", { name = name })
end

function AddMenuButtonHeightSize(parent, name, text, width, height, size)
  local button = Ui_CreateButton(name, "border3_3", text, 0, 0, width, height, 1.0, size)
  UiControl_SetMargin(button, 2, 2)
  UiControl_AddChild(parent, button)
  RegisterUiListener(name, "MenuListener", { name = name })
end

function AddMenuTitle(parent, name, text, width)
  local button = Ui_CreateButton(name, "border3_3_bw", "\\#808080" .. text, 0, 0, width, 35, 1.0, 25)
  UiControl_SetMargin(button, 2, 2)
  UiControl_AddChild(parent, button)
  UiControl_SetActive(button, false)
end

function AddTextLine(parent, text, height, size)
  return AddTextLineLink(parent, text, height, size, '')
end

function AddTextLineLink(parent, text, height, size, event)
  return AddTextLineLinkBorder(parent, text, height, size, event, '')
end

function AddTextLineLinkBorder(parent, text, height, size, event, border)
  local width = MENUS_WIDTH
  local button = Ui_CreateButton(event, border, text, 0, 0, width, height, 1.0, size)
  if event ~= '' then
    RegisterUiListener(event, "LinkListener", { name = event })
  end
  UiControl_SetMargin(button, 2, 2)
  UiControl_AddChild(parent, button)
  UiControl_SetActive(button, event ~= '')
  return button
end

function CreateMenus()
  local bgtop = 0
  local bgbottom = 320
  local x, y, w, h = GetBackgroundOffsetSize()
  if GetPlatformIsiPad() then bgtop = -20; bgbottom = 340; end
  BACKGROUND_MENU_IMAGE = Ui_CreateImageButton("Background1", "intro10c_map_256sq", "", x, y, w, h, 1.0, 18.75, 0, 1, 1, 0, 0)
  BACKGROUND_MAIN_MENU_IMAGE = Ui_CreateImageButton("BackgroundMain", "menuback_partial", "", x, y, w, h, 1.0, 18.75, 0, 1, 0.9375, 0.375, 0)
  UiControl_SetActive(BACKGROUND_MENU_IMAGE, false)    
  UiControl_SetActive(BACKGROUND_MAIN_MENU_IMAGE, false)  
  UiControl_AddChild(CANVAS, BACKGROUND_MENU_IMAGE)
  UiControl_AddChild(CANVAS, BACKGROUND_MAIN_MENU_IMAGE)  
  UiControl_SetVisible(BACKGROUND_MENU_IMAGE, false)
  UiControl_SetVisible(BACKGROUND_MAIN_MENU_IMAGE, false)

  UiInventoryCreate()
  CreateMainMenu()
  CreateCredits()
  CreateStartNewMenu()
  CreateSaveExitMenu()
  CreateLevelExitWaitMenu()
  CreateUpgradeMenus()
  CreateSaveLoadMenus()
  CreateNoDescendMenu()
  CreateReallySaveMenu()
  CreateReallyQuitMenu()
  CreateLoadGameMenu()
  CreateMultiplayerMenu()
  CreateExtrasMenu()
  CreateInGameMenu()
  CreateInGameMenuPage2()
  CreateInGameMapPage()
  CreateInGameAttributesPage()
  CreateInGameStatsPage()  
  CreateInGameQuestsPage()
  CreateInGameSettingsPage()  
  CreateInGameMultiplayerPage()
  CreateInGameCreditsPage()
  CreateInGameDebugPage()
  CreateTurnOffBluetooth()
  CreateTurnOffWiFi()
  CreateConnectFailed()
  CreateGoToMapMenu()
  CreateSkipMenu()
  CreateCharacterTip()
  CreateHealthTip()
  CreateManaTip()
  CreateGoldTip()
  CreateRedKeyTip()
  CreateBlueKeyTip()
  CreateWhiteKeyTip()
  CreateSoulTip()  
  CreateMagicTip()
  CreateTurnTip()
  RegisterUiListener("Resume",  "MenuListener", { name = "Resume" })
  RegisterUiListener("EndGame", "MenuListener", { name = "EndGame" })  
end

function AddRelicTitleTo(menu)
  local x, y, w, h = GetUiBounds()
  local logo = ChooseImage("RelicLogoInGame", "RelicLogoInGame_x2")
  local title = Ui_CreateImageButton("", logo, "", (w - x) * 0.44 + x, y - MENUS_TOP, (w - x) * 1.44 + x, h - MENUS_TOP, 1.0, 25, 0, 1, 1, 0.5, 0)
  UiControl_AddChild(menu, title)
  UiControl_SetActive(title, false)
end

CRYSTAL_BUTTON = nil

function CreateMainMenu()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  MAIN_MENU = CreateMenuType("MainMenu", MENUS_TOP)
  AddMenuTitle(MAIN_MENU, "", "Find The Relic", width)
  AddMenuButton(MAIN_MENU, "MainMenu_StartNewGame", "Start New Game", width)
  LOAD_GAME_BUTTON = AddMenuButton(MAIN_MENU, "MainMenu_LoadGame", "Continue Saved Game", width)
--AddMenuButton(MAIN_MENU, "MainMenu_JoinGame", "Join Multiplayer Game", width)
--AddMenuButton(MAIN_MENU, "MainMenu_Extras", "\\#808080More..", width)
--AddMenuButton(MAIN_MENU, "MainMenu_Alert", "ALERTT!!", width)
  AddMenuButton(MAIN_MENU, "MainMenu_Extras", "More..", width)
  UiControl_LayoutVertical(MAIN_MENU)
  AddRelicTitleTo(MAIN_MENU)
  UiControl_AddChild(CANVAS, MAIN_MENU)
  UiControl_SetVisible(MAIN_MENU, false)
  UiControl_SetActive(MAIN_MENU, false)
end

CRYSTAL_LAST_ACTIVE_STATE = false

function CrystalChangedState(state)
  if not GetPlatformIsiPad() then
    if not state then
	  PlayMusicTheme("AndyLivingston_Heavy")
	else
	  StopMusicTheme()	
	end
  end
end

function UpdateCrystalState()
end

function CreateExtrasMenu()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  EXTRAS_MENU = CreateMenuType("ExtrasMenu", MENUS_TOP)
  AddMenuTitle(EXTRAS_MENU, "",  "More Options", width)
--AddMenuButton(EXTRAS_MENU, "Settings",  "Music and Sound", width)
--AddMenuButton(EXTRAS_MENU, "CrystalAchievements", "Achievements", width)
--AddMenuButton(EXTRAS_MENU, "CrystalLeaderboards", "Leaderboards", width)
  AddMenuButton(EXTRAS_MENU, "Credits",   "Credits", width)
--AddMenuButton(EXTRAS_MENU, "OtherGames",   "Other Games", width)
--AddMenuButton(EXTRAS_MENU, "SendScore",   "Send Score", width)
--AddMenuButton(EXTRAS_MENU, "InviteFriend",   "Invite a Friend", width)  
  AddMenuButton(EXTRAS_MENU, "MainMenu", "Main Menu", width)
  UiControl_LayoutVertical(EXTRAS_MENU)
  AddRelicTitleTo(EXTRAS_MENU)
  UiControl_AddChild(CANVAS, EXTRAS_MENU)
  UiControl_SetVisible(EXTRAS_MENU, false)
end

function CreateLoadGameMenu()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  NEW_LOAD_GAME_MENU = CreateMenuType("LoadGameMenu", MENUS_TOP)
  AddMenuTitle(NEW_LOAD_GAME_MENU, "",  "Continue Saved Game",  width)    
  AddMenuButton(NEW_LOAD_GAME_MENU, "LoadMenu_SinglePlayer", "Single Player",  width)  
  AddMenuButton(NEW_LOAD_GAME_MENU, "LoadMenu_UseBluetooth", "Bluetooth (2 players)",  width)
  AddMenuButton(NEW_LOAD_GAME_MENU, "LoadMenu_UseWiFi", "WiFi (2-4 players)", width)
  AddMenuButton(NEW_LOAD_GAME_MENU, "MainMenu", "\\#808080Main Menu", width)
  UiControl_LayoutVertical(NEW_LOAD_GAME_MENU)
  AddRelicTitleTo(NEW_LOAD_GAME_MENU)
  UiControl_AddChild(CANVAS, NEW_LOAD_GAME_MENU)
  UiControl_SetVisible(NEW_LOAD_GAME_MENU, false)
end

function CreateMultiplayerMenu()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  MULTIPLAYER_MENU = CreateMenuType("MultiplayerMenu", MENUS_TOP)
  AddMenuTitle(MULTIPLAYER_MENU, "",  "Join Multiplayer Game",  width)  
  AddMenuButton(MULTIPLAYER_MENU, "MPMenu_UseBluetooth",  "Bluetooth (2 players)",  width)
  AddMenuButton(MULTIPLAYER_MENU, "MPMenu_UseWiFi",       "WiFi (2-4 players)", width)
  AddMenuButton(MULTIPLAYER_MENU, "MainMenu",      "\\#808080Main Menu", width)
  UiControl_LayoutVertical(MULTIPLAYER_MENU)
  AddRelicTitleTo(MULTIPLAYER_MENU)
  UiControl_AddChild(CANVAS, MULTIPLAYER_MENU)
  UiControl_SetVisible(MULTIPLAYER_MENU, false)
end

function CreateStartNewMenu()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  START_NEW_MENU = CreateMenuType("StartNewMenu", MENUS_TOP)
  AddMenuTitle(START_NEW_MENU, "",  "Start New Game",  width)
  AddMenuButton(START_NEW_MENU, "StartNewMenu_SinglePlayer",  "Single Player",  width)  
  AddMenuButton(START_NEW_MENU, "StartNewMenu_UseBluetooth",  "Bluetooth (2 players)",  width)
  AddMenuButton(START_NEW_MENU, "StartNewMenu_UseWiFi",       "WiFi (2-4 players)", width)
  AddMenuButton(START_NEW_MENU, "MainMenu",      "\\#808080Main Menu", width)
  UiControl_LayoutVertical(START_NEW_MENU)
  AddRelicTitleTo(START_NEW_MENU)
  UiControl_AddChild(CANVAS, START_NEW_MENU)
  UiControl_SetVisible(START_NEW_MENU, false)
end

function CreateSaveExitMenu()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  SAVE_EXIT_MENU = CreateMenuType("StartNewMenu", MENUS_TOP - 25)
  AddMenuTitle(SAVE_EXIT_MENU, "",  "Save/Exit Game",  width)
  AddMenuButton(SAVE_EXIT_MENU, "Resume",  "Resume",  width)
  AddMenuButton(SAVE_EXIT_MENU, "SaveGame",  "Save Game",  width)
  AddMenuButton(SAVE_EXIT_MENU, "ExitGame", "Quit", width)
  --local slider = CreateSlider(40, "slider1")
  --UiControl_AddChild(SAVE_EXIT_MENU, slider)
  UiControl_LayoutVertical(SAVE_EXIT_MENU)
  UiControl_AddChild(CANVAS, SAVE_EXIT_MENU)
  UiControl_SetVisible(SAVE_EXIT_MENU, false)
end

function CreateLevelExitWaitMenu()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  LEVEL_EXIT_WAIT_MENU = Ui_CreateMenu("LevelExitWaitMenu", "border3", "..", 240 - halfWidth, MENUS_TOP - 25, 240 + halfWidth, MENUS_TOP - 25, 1.0, 22.5)
  AddMenuTitle(LEVEL_EXIT_WAIT_MENU, "", "Exit Level: Wait for others?", width)
  AddMenuButton(LEVEL_EXIT_WAIT_MENU, "WaitForAllPlayers", "Wait", width)
  AddMenuButton(LEVEL_EXIT_WAIT_MENU, "Resume", "Cancel", width)
  UiControl_LayoutVertical(LEVEL_EXIT_WAIT_MENU)
  UiControl_AddChild(CANVAS, LEVEL_EXIT_WAIT_MENU)
  UiControl_SetVisible(LEVEL_EXIT_WAIT_MENU, false)
end

function CreateNoDescendMenu()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  NO_DESCEND_MENU = Ui_CreateMenu("NoDescendMenu", "border3", "..", 240 - halfWidth, MENUS_TOP - 25, 240 + halfWidth, MENUS_TOP - 25, 1.0, 22.5)
  AddMenuTitle(NO_DESCEND_MENU, "", "You may not descend.", width)
  AddMenuButton(NO_DESCEND_MENU, "Resume", "Dismiss", width)
  UiControl_LayoutVertical(NO_DESCEND_MENU)
  UiControl_AddChild(CANVAS, NO_DESCEND_MENU)
  UiControl_SetVisible(NO_DESCEND_MENU, false)
end

function CreateGoToMapMenu()
  local width = 180
  local halfWidth = width * 0.5
  
  MAP_CONTAINER_MENU = Ui_CreateMenu("MapContainerMenu", "border3", "..", 240 - halfWidth, MENUS_TOP - 25, 240 + halfWidth, MENUS_TOP - 25, 1.0, 22.5)
  
  local button = Ui_CreateButton("BackgroundBackButton", "blank", " ", -1000, -1000, 1000, 1000, 1.0, 22.5)
  RegisterUiListener("BackgroundBackButton", "MenuListener", { name = "FocusBack" })
  UiControl_AddChild(MAP_CONTAINER_MENU, button)
  
  GO_TO_MAP_MENU = Ui_CreateMenu("GoToMapMenu", "border3", "..", 240 - halfWidth, MENUS_TOP - 25, 240 + halfWidth, MENUS_TOP - 25, 1.0, 22.5)  
 
  AddMenuButton(GO_TO_MAP_MENU, "GoToMap", "Map", width)
  button = Ui_CreateButton("$tmp0234", "blank", " ", 0, 0, width, 20, 1.0, 22.5)  
  RegisterUiListener("$tmp0234", "MenuListener", { name = "Back" })
  UiControl_AddChild(GO_TO_MAP_MENU, button)
  
  AddMenuButton(GO_TO_MAP_MENU, "SaveGame", "Save Game", width)  
  AddMenuButton(GO_TO_MAP_MENU, "ExitGame", "Quit", width)
  
  UiControl_LayoutVertical(GO_TO_MAP_MENU)
  
  UiControl_AddChild(GO_TO_MAP_MENU, MAP_CONTAINER_MENU)
  
  UiControl_AddChild(CANVAS, GO_TO_MAP_MENU)
  UiControl_SetVisible(GO_TO_MAP_MENU, false)
end

function CreateSkipMenu()
  local width = 100
  local halfWidth = width * 0.5
  
  local container = Ui_CreateMenu("MapContainerMenu", "border3", "..", 240 - halfWidth, MENUS_TOP - 25, 240 + halfWidth, MENUS_TOP - 25, 1.0, 22.5)  
  local button = Ui_CreateButton("BackgroundBackButton", "blank", " ", -1000, -1000, 1000, 1000, 1.0, 22.5)
  RegisterUiListener("BackgroundBackButton", "MenuListener", { name = "FocusBack" })
  UiControl_AddChild(container, button)
  
  SKIP_MENU = Ui_CreateMenu("SkipMenu", "border3", "..", 240 - halfWidth, MENUS_TOP - 25, 240 + halfWidth, MENUS_TOP - 25, 1.0, 22.5)  
 
  AddMenuButton(SKIP_MENU, "GoToMap", "Skip?", width)
  
  UiControl_LayoutVertical(SKIP_MENU)
  UiControl_AddChild(SKIP_MENU, container)  
  UiControl_AddChild(CANVAS, SKIP_MENU)
  UiControl_SetVisible(SKIP_MENU, false)
end

function CreateSlider(elements, name)
  local width = MENUS_WIDTH
  local slider = Ui_CreateMenu(name, "border3", "", 100, 100, width, 0, 1.0, 22.5)
  for i=1,elements do
    AddMenuButton(slider, name .. "#" .. i, " ", 20)
  end
  UiControl_LayoutHorizontal(slider)
  return slider

end

function CreateCharacterTip()
  CHARACTER_TIP = CreateTipByName("CharacterTip", "Tap your warrior to pause and\nshow the map.")
end

function CreateTurnTip()
  TURN_TIP = CreateTipByName("TurnTip", "You found a turn spell!\n\nThe turn spell turns your enemies\nagainst each other for a few seconds.\n\nDon't get in their way!")
end

function CreateHealthTip()
  HEALTH_TIP = CreateTipByName("HealthTip", "You found a health potion!\n\nTap the potion button on the right\nhand side to replenish your health.")
end

function CreateManaTip()
  MANA_TIP = CreateTipByName("ManaTip", "You found a mana potion!\n\nThis potion replenishes your magic ability.")
end

function CreateGoldTip()
  --GOLD_TIP = CreateTipByName("GoldTip", "You found gold!\n\nAs you continue to collect gold,\nthe counter on the left side increases.\n\nGold serves as the score in The Relic.")
  GOLD_TIP = CreateTipByName("GoldTip", "You found gold!")
end

function CreateRedKeyTip()
  REDKEY_TIP = CreateTipByName("RedKeyTip", "You found a \\#ff8080red\\#ffffff key!\n\nWith this you can open one door\nmarked with \\#ff8080red\\#ffffff text.")
end

function CreateBlueKeyTip()
  BLUEKEY_TIP = CreateTipByName("BlueKeyTip", "You found a \\#8080ffblue\\#ffffff key!\n\nWith this you can open one door\nmarked with \\#8080ffblue\\#ffffff text.")
end

function CreateWhiteKeyTip()
  WHITEKEY_TIP = CreateTipByName("BlueKeyTip", "You found a white key!\n\nWith this you can open one door\nmarked with white text.")
end

function CreateSoulTip()
  SOUL_TIP = CreateTipByName("SoulTip", "Collect souls to heal yourself!")
end

function CreateMagicTip()
  MAGIC_TIP = CreateTipByName("MagicTip", "Tap the magic ring to release a wave\nof destructive energy.")
end

function CreateTipByName(name, text)
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
   
  local tip = Ui_CreateMenu(name, "border3", "..", 240 - halfWidth, MENUS_TOP - 200, 240 + halfWidth, MENUS_TOP - 25, 1.0, 22.5)
  AddTip(tip, "Back", text .. "\n\n\\#408040[dismiss]", width, 400)
  local button = Ui_CreateButton("$tmp_" .. name, "blank", " ", 0, 0, width, 20, 1.0, 22.5)  
  RegisterUiListener("$tmp_" .. name, "MenuListener", { name = "Back" })
  UiControl_AddChild(tip, button)
  UiControl_LayoutVertical(tip)  
  UiControl_AddChild(CANVAS, tip)
  UiControl_SetVisible(tip, false)
  return tip
end

function CreateReallySaveMenu()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  REALLY_SAVE_MENU = Ui_CreateMenu("ReallySaveMenu", "border3", "..", 240 - halfWidth, MENUS_TOP - 25, 240 + halfWidth, MENUS_TOP - 25, 1.0, 22.5)
  AddMenuTitle(REALLY_SAVE_MENU, "",  "Save Game: Are you sure?",  width)
  AddMenuButton(REALLY_SAVE_MENU, "YesSaveGame", "Yes",  width)
  AddMenuButton(REALLY_SAVE_MENU, "NoSaveOrQuitGame", "No",  width)
  UiControl_LayoutVertical(REALLY_SAVE_MENU)
  UiControl_AddChild(CANVAS, REALLY_SAVE_MENU)
  UiControl_SetVisible(REALLY_SAVE_MENU, false)
end

function CreateReallyQuitMenu()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  REALLY_QUIT_MENU = Ui_CreateMenu("ReallyQuitMenu", "border3", "..", 240 - halfWidth, MENUS_TOP - 25, 240 + halfWidth, MENUS_TOP - 25, 1.0, 22.5)
  AddMenuTitle(REALLY_QUIT_MENU, "", "Quit Game: Are you sure?", width)
  AddMenuButton(REALLY_QUIT_MENU, "EndGame", "Yes", width)
  AddMenuButton(REALLY_QUIT_MENU, "NoSaveOrQuitGame", "No", width)
  UiControl_LayoutVertical(REALLY_QUIT_MENU)
  UiControl_AddChild(CANVAS, REALLY_QUIT_MENU)
  UiControl_SetVisible(REALLY_QUIT_MENU, false)
end

function CreateCredits()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  local top = 0
  local height = 20
  local size = 23
  local dividerHeight = 15
  CREDITS_DISMISS_BG = Ui_CreateButton("EndCredits", "blank", "", 0, 0, 320, 480, 0.1, 25)
  RegisterUiListener("EndCredits", "MenuListener", { name = "EndCredits" })
  CREDITS_SCREEN = Ui_CreateMenu("CreditsControl", "", "..", 240 - halfWidth, top, 240 + halfWidth, top, 1.0, 22.5)
  AddTextLine(CREDITS_SCREEN, "\\#808080The Relic", height * 2.0, size * 2.0)
  AddTextLine(CREDITS_SCREEN, "", dividerHeight * 4, size)
  AddTextLine(CREDITS_SCREEN, "\\#808080A Paul Senzee Game", height, size)
  AddTextLine(CREDITS_SCREEN, "", dividerHeight * 1, size)
  AddTextLine(CREDITS_SCREEN, "\\#808080developed by", height, size)
  AddTextLineLink(CREDITS_SCREEN, "Axolotl Studios", height, size, "LINK_AxolotlStudios")
  AddTextLine(CREDITS_SCREEN, "", dividerHeight, size)
  AddTextLine(CREDITS_SCREEN, "\\#808080published by", height, size)
  AddTextLineLink(CREDITS_SCREEN, "Chillingo", height, size, "LINK_Chillingo")
  AddTextLine(CREDITS_SCREEN, "", dividerHeight, size)
  AddTextLine(CREDITS_SCREEN, "\\#808080design, programming & art", height, size)
  AddTextLineLink(CREDITS_SCREEN, "Paul Senzee", height, size, "LINK_AxolotlStudios")
  AddTextLine(CREDITS_SCREEN, "", dividerHeight, size)
  AddTextLine(CREDITS_SCREEN, "\\#808080character art & animation", height, size)
  AddTextLine(CREDITS_SCREEN, "Mark Jones", height, size)
  AddTextLine(CREDITS_SCREEN, "", dividerHeight, size)
  AddTextLine(CREDITS_SCREEN, "\\#808080original songs", height, size)
  AddTextLineLink(CREDITS_SCREEN, "Andy Livingston", height, size, "LINK_iTunes_AndyLivingston")
  AddTextLineLink(CREDITS_SCREEN, "\\#808080heavy \\#8080ff(itunes)\\#ffffff", height, size, "LINK_iTunes_Heavy")
  AddTextLineLink(CREDITS_SCREEN, "\\#808080indigo winter \\#8080ff(itunes)\\#ffffff", height, size, "LINK_iTunes_IndigoWinter")
  AddTextLine(CREDITS_SCREEN, "", dividerHeight, size)
  AddTextLine(CREDITS_SCREEN, "\\#808080scene art", height, size)  
  AddTextLine(CREDITS_SCREEN, "\\#808080adapted from the public domain", height, size)
  AddTextLine(CREDITS_SCREEN, "\\#808080retrieved from wikimedia commons", height, size)
  AddTextLineLink(CREDITS_SCREEN, "\\#808080commons.wikimedia.org", height, size, "LINK_WikimediaCommons")
  AddTextLine(CREDITS_SCREEN, "", dividerHeight, size)
  AddTextLine(CREDITS_SCREEN, "\\#808080chillingo", height, size)
  AddTextLine(CREDITS_SCREEN, "Chris Byatte", height, size)
  AddTextLine(CREDITS_SCREEN, "Tom Kinniburgh", height, size)
  AddTextLine(CREDITS_SCREEN, "Lucy Blundell", height, size)  
  AddTextLine(CREDITS_SCREEN, "Gareth Reese", height, size)  
  AddTextLine(CREDITS_SCREEN, "Jamie Mccubbin", height, size)
  AddTextLine(CREDITS_SCREEN, "Carmen Pearson", height, size)
  AddTextLine(CREDITS_SCREEN, "Travis Birdsall", height, size)
  AddTextLine(CREDITS_SCREEN, "James Parr", height, size)  
  AddTextLine(CREDITS_SCREEN, "Adam Barker", height, size)  
  AddTextLine(CREDITS_SCREEN, "", dividerHeight * 1, size)  
  AddTextLine(CREDITS_SCREEN, "\\#808080testing", height, size)
  AddTextLine(CREDITS_SCREEN, "Eric Lambert", height, size)
  AddTextLine(CREDITS_SCREEN, "Ross T. Johnson", height, size)  
  AddTextLine(CREDITS_SCREEN, "Zach Lips", height, size)
  AddTextLine(CREDITS_SCREEN, "", dividerHeight * 1, size)  
  AddTextLine(CREDITS_SCREEN, "Jonathan Payne", height, size)
  AddTextLine(CREDITS_SCREEN, "Andrzej (Andy) Raczynski", height, size)  
  AddTextLine(CREDITS_SCREEN, "Jason Boucher", height, size) 
  AddTextLine(CREDITS_SCREEN, "Albert Fong", height, size)   
  AddTextLine(CREDITS_SCREEN, "Paul Lind", height, size)          
  AddTextLine(CREDITS_SCREEN, "Mason Cooper", height, size)
  AddTextLine(CREDITS_SCREEN, "Kenneth Petro", height, size)
  AddTextLine(CREDITS_SCREEN, "Ryan Harmon", height, size)  
  AddTextLine(CREDITS_SCREEN, "Scott Hedges", height, size)
  AddTextLine(CREDITS_SCREEN, "John Loner", height, size)
  AddTextLine(CREDITS_SCREEN, "Jeff Yue", height, size)
  AddTextLine(CREDITS_SCREEN, "", dividerHeight * 3, size)  
  AddTextLine(CREDITS_SCREEN, "\\#808080special thanks", height, size)
  AddTextLine(CREDITS_SCREEN, "", dividerHeight * 1, size)    
  AddTextLine(CREDITS_SCREEN, "\\#808080my wonderful and very patient wife", height, size)
  AddTextLine(CREDITS_SCREEN, "\\#808080(and principal tester), \\#ffffff Rebekka Senzee", height, size)
  AddTextLine(CREDITS_SCREEN, "", dividerHeight * 1, size) 
  AddTextLine(CREDITS_SCREEN, "\\#808080my children (also testers)\\#ffffff", height, size)
  AddTextLine(CREDITS_SCREEN, "Dylan\\#808080,\\#ffffff Gabrielle\\#808080,\\#ffffff  Caroline \\#808080and\\#ffffff Kayla", height, size)  
--AddTextLine(CREDITS_SCREEN, "\\#808080and death shall have no dominion", height, size)  
  AddTextLine(CREDITS_SCREEN, "", dividerHeight * 5, size)
  AddTextLine(CREDITS_SCREEN, "\\#808080[tap to dismiss]", height, size)    
  UiControl_LayoutVertical(CREDITS_SCREEN)
  UiControl_AddChild(CANVAS, CREDITS_DISMISS_BG)  
  UiControl_AddChild(CANVAS, CREDITS_SCREEN)
  UiControl_SetVisible(CREDITS_SCREEN, false)
  UiControl_SetVisible(CREDITS_DISMISS_BG, false)
end

function CreateTurnOffWiFi()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  local height = 20
  local size = 23
  local dividerHeight = 15
  TURNOFFWIFI_SCREEN = Ui_CreateMenu("TurnOffWiFiControl", "", "..", 240 - halfWidth, MENUS_TOP - 50, 240 + halfWidth, MENUS_TOP + height, 1.0, 22.5)
  AddTextLine(TURNOFFWIFI_SCREEN, "Please ensure that WiFi is disabled in\norder to use Bluetooth.", 80, size)
  AddMenuButton(TURNOFFWIFI_SCREEN, "ContinueToMultiplayer", "Continue", width)
  AddMenuButton(TURNOFFWIFI_SCREEN, "MainMenu", "\\#808080Main Menu", width)
  UiControl_LayoutVertical(TURNOFFWIFI_SCREEN)
  UiControl_AddChild(CANVAS, TURNOFFWIFI_SCREEN)
  UiControl_SetVisible(TURNOFFWIFI_SCREEN, false)
end

function CreateTurnOffBluetooth()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  local height = 20
  local size = 23
  local dividerHeight = 15
  TURNOFFBLUETOOTH_SCREEN = Ui_CreateMenu("TurnOffBluetoothControl", "", "..", 240 - halfWidth, MENUS_TOP - 50, 240 + halfWidth, MENUS_TOP + height, 1.0, 22.5)
  AddTextLine(TURNOFFBLUETOOTH_SCREEN, "Please ensure that Bluetooth is disabled in\norder to use WiFi.", 80, size)
  AddMenuButton(TURNOFFBLUETOOTH_SCREEN, "ContinueToMultiplayer", "Continue", width)
  AddMenuButton(TURNOFFBLUETOOTH_SCREEN, "MainMenu", "\\#808080Main Menu", width)
  UiControl_LayoutVertical(TURNOFFBLUETOOTH_SCREEN)
  UiControl_AddChild(CANVAS, TURNOFFBLUETOOTH_SCREEN)
  UiControl_SetVisible(TURNOFFBLUETOOTH_SCREEN, false)
end

function CreateConnectFailed()
  local width = MENUS_WIDTH
  local halfWidth = width * 0.5
  local height = 20
  local size = 23
  local dividerHeight = 15
  CONNECTFAILED_SCREEN = Ui_CreateMenu("ConnectFailedControl", "", "..", 240 - halfWidth, MENUS_TOP, 240 + halfWidth, MENUS_TOP + height, 1.0, 22.5)
  AddTextLine(CONNECTFAILED_SCREEN, "Failed to connect to host.", 80, size)
  AddMenuButton(CONNECTFAILED_SCREEN, "MainMenu", "Main Menu", width)
  UiControl_LayoutVertical(CONNECTFAILED_SCREEN)
  AddRelicTitleTo(CONNECTFAILED_SCREEN)
  UiControl_AddChild(CANVAS, CONNECTFAILED_SCREEN)
  UiControl_SetVisible(CONNECTFAILED_SCREEN, false)
end

function AddHorizMenuButton(parent, name, text, width)
  local button = Ui_CreateButton(name, "border88_dark", text, 0, HEIGHT - 20, width, HEIGHT + 5, 1.0, 18.75)
  UiControl_AddChild(parent, button)
  RegisterUiListener(name, "MenuListener", { name = name })
end

function ShowInGameElements()
  _SetVisible(IN_GAME_MENU, true)
  _SetVisible(BACKGROUND_MENU_IMAGE, true)
end

function ShowInGameElementsPage2()
  _SetVisible(IN_GAME_MENU2, true)
  _SetVisible(BACKGROUND_MENU_IMAGE, true)
end

function HideInGameMenu()
  HideMenus()
  UiInventoryHide()
  _SetVisible(BACKGROUND_MENU_IMAGE, false)
  _SetVisible(IN_GAME_MENU, false)
  _SetVisible(IN_GAME_MENU2, false)
  _SetVisible(MAP_PAGE, false)
  _SetVisible(ATTRIBUTES_PAGE, false)
  _SetVisible(STATS_PAGE, false)
  _SetVisible(QUESTS_PAGE, false)
  _SetVisible(SETTINGS_PAGE, false)
  _SetVisible(MULTIPLAYER_PAGE, false)
  _SetVisible(CREDITS_PAGE, false)
  _SetVisible(DEBUG_PAGE, false)
end

function CreateInGameMenu()
  local x, y, w, h = GetUiBounds()
  IN_GAME_MENU = Ui_CreateMenu("InGameMenu", "border3", "..", x, y, w, y + 30, 1.0, 22.5)

--AddHorizMenuButton(IN_GAME_MENU, "To_Map",   "Map",   70)  
--AddHorizMenuButton(IN_GAME_MENU, "To_Items", "Items", 80)  
--AddHorizMenuButton(IN_GAME_MENU, "To_Attributes", "Attributes", 130)  
--AddHorizMenuButton(IN_GAME_MENU, "To_Stats", "Stats", 80)
--AddHorizMenuButton(IN_GAME_MENU, "To_Quests", "Quests", 100)
  AddHorizMenuButton(IN_GAME_MENU, "To_NextIGPage", ">",   20)  
  UiControl_LayoutHorizontal(IN_GAME_MENU)
  
  UiControl_AddChild(CANVAS, IN_GAME_MENU)
  UiControl_SetVisible(IN_GAME_MENU, false)
end

function CreateInGameMenuPage2()
  IN_GAME_MENU2 = Ui_CreateMenu("InGameMenu2", "border3", "..", 0, HEIGHT - 30, WIDTH, HEIGHT, 1.0, 22.5)
  
  AddHorizMenuButton(IN_GAME_MENU2, "To_PrevIGPage", "<",   20)
--AddHorizMenuButton(IN_GAME_MENU2, "To_Settings", "Settings", 110)
--AddHorizMenuButton(IN_GAME_MENU2, "To_Multiplayer", "Multiplayer", 150)
--AddHorizMenuButton(IN_GAME_MENU2, "To_Credits", "Credits", 100)
  AddHorizMenuButton(IN_GAME_MENU2, "To_$DEBUG$", "$debug$", 100)
  UiControl_LayoutHorizontal(IN_GAME_MENU2)
  
  UiControl_AddChild(CANVAS, IN_GAME_MENU2)
  UiControl_SetVisible(IN_GAME_MENU2, false)  
end

function AddPauseMenuButtonR2L(page, index, event, name)
  local x, y, w, h = GetUiBounds()
  local BUTTON_WIDTH = 90
  y = y + 15
  w = w - BUTTON_WIDTH * 2
  local offset = BUTTON_WIDTH * index
  local button = Ui_CreateButton(event, "darkborder", name, w - offset, y, (w + BUTTON_WIDTH / 2 + 10) - offset, y + 40, 1.0, 30.0)
  UiControl_AddChild(page, button)
end

function AddResumeButton(page)
  AddPauseMenuButtonR2L(page, 0, "Resume", "resume")
end

function CreateInGameMapPage()
  local x, y, w, h = GetUiBounds()
  MAP_PAGE = Ui_CreateMenu("MapPage", "border3", "..", x, y, w, h, 1.0, 22.5)
  local title = Ui_CreateText("Map", "Pause", 20, TITLE_START, 80, TITLE_START + 50, 1.0, 30.0, false)
  AddPauseMenuButtonR2L(MAP_PAGE, 0, "Resume", "resume")
  AddPauseMenuButtonR2L(MAP_PAGE, 1, "SaveGame", "save")
  AddPauseMenuButtonR2L(MAP_PAGE, 2, "ExitGame", "quit")
  UiControl_AddChild(MAP_PAGE, title)
  UiControl_AddChild(CANVAS, MAP_PAGE)  
  UiControl_SetVisible(MAP_PAGE, false)
end

function AddAttributeControl(parent, name, value, namewidth)
  local selection = Ui_CreateMenu(name .. "-box", "border3", "..", 0, 0, 200, 22, 1.0, 22.5)
  local nameText = Ui_CreateText(name, name, 0, 0, namewidth, 20, 1.0, 24, false)
  UiControl_AddChild(selection, nameText)
  UiControl_SetActive(nameText, false)
  local valueText = Ui_CreateText(name .. "-value", tostring(value), namewidth, 0, namewidth + 20, 20, 1.0, 24, false)
  UiControl_SetActive(valueText, false)
  UiControl_AddChild(selection, valueText)
  UiControl_LayoutHorizontal(selection)
  RegisterUiListener(name, "AttributeListener", { name = name })
  UiControl_AddChild(parent, selection)
  return valueText
end

TOTAL_POINTS_TO_DISTRIBUTE = 5
TOTAL_POINTS_DISTRIBUTED   = 0
CONTROLUPDOWN_VALUES       = {}
CONTROLUPDOWN_UPS          = {}

function ControlUpDownSum()
  local sum = 0
  for k, v in pairs(CONTROLUPDOWN_VALUES) do
    local number = UiControl_GetText(v)
    sum = sum + number  
  end
  return sum
end

function UpdateControlUpDown()
  local sum  = ControlUpDownSum()
  local show = (sum < TOTAL_POINTS_TO_DISTRIBUTE)
  for k, v in pairs(CONTROLUPDOWN_UPS) do
    UiControl_SetVisible(v, show)
  end  
end

function ControlUpDownListener(info, data)
  if info == 'SELECTED' then
    local number = UiControl_GetText(data.valueText)
    local sum    = ControlUpDownSum()
    number = number + data.direction
    if number < 0 then
      number = 0
    end
    UiControl_SetText(data.valueText, tostring(number))
    UiControl_SetVisible(data.down, number ~= 0)    
    UpdateControlUpDown()
  end
end

function AddUpDownControl(name)
  local control = Ui_CreateMenu(name .. "-updown", "border3", "..", 0, 0, 200, 20, 1.0, 22.5)
  local up      = Ui_CreateImageButton(name .. "-up",   "plus1_small", "",  250, -12, 270, 7, 1.0, 18.75, 0, 1, 1, 0, 0)
  local numeric = Ui_CreateText(name .. "-number-value", tostring("0"), 280, 0, 300, 20, 1.0, 18.75, true)
  local down    = Ui_CreateImageButton(name .. "-down", "minus1_small", "", 290, -12, 310, 7, 1.0, 18.75, 0, 1, 1, 0, 0)
  UiControl_AddChild(control, up)
  UiControl_AddChild(control, numeric)
  UiControl_AddChild(control, down)
  UiControl_SetVisible(down, false)
  CONTROLUPDOWN_VALUES[name] = numeric
  CONTROLUPDOWN_UPS[name] = up
  RegisterUiListener(name .. "-up",   "ControlUpDownListener", { name = name .. "-number-value", valueText = numeric, direction =  1, up = up, down = down })
  RegisterUiListener(name .. "-down", "ControlUpDownListener", { name = name .. "-number-value", valueText = numeric, direction = -1, up = up, down = down })
  return control
end

function AddAttributeControlAdjustable(parent, name, value)
  local selection = Ui_CreateMenu(name .. "-box", "border3", "..", 0, 0, 200, 20, 1.0, 22.5)
  local nameText = Ui_CreateText(name, name, 0, 0, 150, 20, 1.0, 18.75, false)
  UiControl_AddChild(selection, nameText)
  UiControl_SetActive(nameText, false)
  local valueText = Ui_CreateText(name .. "-value", tostring(value), 150, 0, 200, 20, 1.0, 18.75, false)
  local valueControl = AddUpDownControl(name)
  UiControl_SetActive(valueText, false)
  UiControl_AddChild(selection, valueText)
  UiControl_LayoutHorizontal(selection)    
  UiControl_AddChild(selection, valueControl)
  RegisterUiListener(name, "AttributeListener", { name = name })
  UiControl_AddChild(parent, selection)  
  return valueText
end  

LVL_CONTROL = nil
HP_CONTROL  = nil
EXP_CONTROL = nil
STR_CONTROL = nil
DEX_CONTROL = nil
VIT_CONTROL = nil
ENG_CONTROL = nil
LCK_CONTROL = nil
LEVEL_UP_MODE = false

function PopulateAttributesPage(player)

  if not player then
    return
  end
  
  local data = player:data()  

  local attrs    = Ui_CreateMenu("attributes-box", "border3", "..", 20, TITLE_START + 50, 220, TITLE_START + 80, 1.0, 22.5)
  LVL_CONTROL    = AddAttributeControl(attrs, "Level",      tostring(player:GetLevel()), 130)  
  EXP_CONTROL    = AddAttributeControl(attrs, "Experience", tostring(player:GetExperience()), 130)
  HP_CONTROL     = AddAttributeControl(attrs, "Hit Points", tostring(player:GetHitPoints()) .. "/" .. tostring(player:GetMaxHitPoints()), 130)  
                   AddAttributeControl(attrs, "\\#404040-", "", 130)

  STR_CONTROL    = AddAttributeControl(attrs, "Strength",   "\\#40ff40" .. player:GetStrength()  .. "", 130)
  DEX_CONTROL    = AddAttributeControl(attrs, "Dexterity",  "\\#40ff40" .. player:GetDexterity() .. "\\#ffffff", 130)
  VIT_CONTROL    = AddAttributeControl(attrs, "Vitality",   "\\#40ff40" .. player:GetVitality()  .. "\\#ffffff", 130)
  ENG_CONTROL    = AddAttributeControl(attrs, "Energy",     "\\#40ff40" .. player:GetEnergy()    .. "\\#ffffff", 130)
  LCK_CONTROL    = AddAttributeControl(attrs, "Luck",       "\\#40ff40" .. player:GetLuck()      .. "\\#ffffff", 130)

  local stats   = Ui_CreateMenu("stats-box", "border3", "..", 230, TITLE_START + 50, 420, TITLE_START + 80, 1.0, 22.5)
                  AddAttributeControl(stats, "\\#404040-", "", 220)  
                  AddAttributeControl(stats, "\\#404040-", "", 220)
                  AddAttributeControl(stats, "\\#404040-", "", 220)
                  AddAttributeControl(stats, "\\#404040-", "", 220)                  
  STATS_KILLED  = AddAttributeControl(stats, "Killed", tostring(data.kills), 200)
  STATS_SKELS   = AddAttributeControl(stats, "\\#808080  Skeletons",    tostring(data.skeletonsKilled), 200)  
  STATS_NECROS  = AddAttributeControl(stats, "\\#808080  Necromancers", tostring(data.necromancersKilled), 200)
  STATS_SPIDERS = AddAttributeControl(stats, "\\#808080  Spiders",      tostring(data.spidersKilled), 200)
  STATS_QUINOS  = AddAttributeControl(stats, "\\#808080  Quinotaurs",   tostring(data.quinotaursKilled), 200)  
  
  UiControl_LayoutVertical(attrs)
  UiControl_LayoutVertical(stats)  
  UiControl_AddChild(ATTRIBUTES_PAGE, attrs)
  UiControl_AddChild(ATTRIBUTES_PAGE, stats)  
end

function CompleteLevelUpListener()
  LEVEL_UP_MODE = false
  UiControl_SetVisible(LEVEL_UP_BUTTON, false)
  local player = GetPlayer()
  player:SetLevel(player:GetLevelFromExperience())
  MenuListener('SELECTED', { name = "Resume" })
end

function SetLevelUpMode(value)
  LEVEL_UP_MODE = value
end

HP_COLORS = { "\\#ff0000", "\\#ff8000", "\\#ffff00", "\\#80ff00", "\\#00ff00", "\\#00ff00" }

function UpdateAttributes(player)
  
  if not player then
    return
  end
  
  if LVL_CONTROL == nil then
    PopulateAttributesPage(player)
  end

  local data = player:data()
  
  UiControl_SetText(LVL_CONTROL, "\\#8080ff" .. player:GetLevel())
  local hp = math.floor(player:GetHitPoints())
  local maxhp = math.floor(player:GetMaxHitPoints())
  local hpratio = hp / maxhp
  local color = HP_COLORS[math.floor(hpratio * (#HP_COLORS - 1)) + 1]  
  UiControl_SetText(HP_CONTROL,  color .. hp .. "\\#ffffff/" .. tostring(maxhp))
  UiControl_SetText(EXP_CONTROL, tostring(player:GetExperience()) .. "   \\#808080(\\#8080ff" .. tostring(player:GetNextLevelExperience() - player:GetExperience()) .. "\\#808080 to next)")
  UiControl_SetText(STR_CONTROL, "\\#40ff40" .. player:GetStrength())
  UiControl_SetText(DEX_CONTROL, "\\#40ff40" .. player:GetDexterity())
  UiControl_SetText(VIT_CONTROL, "\\#40ff40" .. player:GetVitality())
  UiControl_SetText(ENG_CONTROL, "\\#40ff40" .. player:GetEnergy())
  UiControl_SetText(LCK_CONTROL, "\\#40ff40" .. player:GetLuck())

  UiControl_SetText(STATS_KILLED,  tostring(data.kills))
  UiControl_SetText(STATS_SKELS,   tostring(data.skeletonsKilled))
  UiControl_SetText(STATS_NECROS,  tostring(data.necromancersKilled))
  UiControl_SetText(STATS_SPIDERS, tostring(data.spidersKilled))
  UiControl_SetText(STATS_QUINOS,  tostring(data.quinotaursKilled))  
  
--_SetVisible(IN_GAME_MENU, not LEVEL_UP_MODE)
  
end

STATS_KILLED  = nil
STATS_SKELS   = nil
STATS_NECROS  = nil
STATS_SPIDERS = nil
STATS_QUINOS  = nil

function CreateInGameAttributesPage()
  ATTRIBUTES_PAGE = Ui_CreateMenu("AttrPage", "border3", "..", 0, 0, WIDTH, HEIGHT, 1.0, 22.5)
  local title = Ui_CreateText("Attributes", "Attributes", 20, TITLE_START, 80, TITLE_START + 50, 1.0, 30.0, false) 
  AddResumeButton(ATTRIBUTES_PAGE)
  UiControl_AddChild(ATTRIBUTES_PAGE, title)
  UiControl_AddChild(CANVAS, ATTRIBUTES_PAGE)
  PopulateAttributesPage(GetPlayer())  
  UiControl_SetVisible(ATTRIBUTES_PAGE, false)
end

function CreateInGameStatsPage()
  STATS_PAGE = Ui_CreateMenu("StatsPage", "border3", "..", 0, 0, WIDTH, HEIGHT, 1.0, 22.5)
  local title = Ui_CreateText("Stats", "Stats", 20, TITLE_START, 80, TITLE_START + 50, 1.0, 30.0, false) 
  AddResumeButton(STATS_PAGE)    
  UiControl_AddChild(STATS_PAGE, title)
  UiControl_AddChild(CANVAS, STATS_PAGE)
  UiControl_SetVisible(STATS_PAGE, false)
end

function CreateInGameQuestsPage()
  QUESTS_PAGE = Ui_CreateMenu("QuestsPage", "border3", "..", 0, 0, WIDTH, HEIGHT, 1.0, 22.5)
  local title = Ui_CreateText("Quests", "Quests", 20, TITLE_START, 80, TITLE_START + 50, 1.0, 30.0, false) 
  AddResumeButton(QUESTS_PAGE)    
  UiControl_AddChild(QUESTS_PAGE, title)
  UiControl_AddChild(CANVAS, QUESTS_PAGE)
  UiControl_SetVisible(QUESTS_PAGE, false)
end

function CreateInGameSettingsPage()
  SETTINGS_PAGE = Ui_CreateMenu("SettingsPage", "border3", "..", 0, 0, WIDTH, HEIGHT, 1.0, 22.5)
  local title = Ui_CreateText("Settings", "Settings", 20, TITLE_START, 80, TITLE_START + 50, 1.0, 30.0, false) 
  AddResumeButton(SETTINGS_PAGE)    
  UiControl_AddChild(SETTINGS_PAGE, title)
  UiControl_AddChild(CANVAS, SETTINGS_PAGE)
  UiControl_SetVisible(SETTINGS_PAGE, false)
end

function CreateInGameMultiplayerPage()
  MULTIPLAYER_PAGE = Ui_CreateMenu("MultiplayerPage", "border3", "..", 0, 0, WIDTH, HEIGHT, 1.0, 22.5)
  local title = Ui_CreateText("Multiplayer", "Multiplayer", 20, TITLE_START, 80, TITLE_START + 50, 1.0, 30.0, false) 
  AddResumeButton(MULTIPLAYER_PAGE)
  UiControl_AddChild(MULTIPLAYER_PAGE, Ui_CreateButton("StartBluetoothPicker", "border88_dark", "Bluetooth (2 players)", 40, 120, 260, 140, 1.0, 18.75))
  RegisterUiListener("StartBluetoothPicker", "MenuListener", { name = "StartBluetoothPicker" })  
  UiControl_AddChild(MULTIPLAYER_PAGE, Ui_CreateButton("StartWiFiPicker", "border88_dark", "WiFi (4 players)", 40, 150, 260, 170, 1.0, 18.75))
  RegisterUiListener("StartWiFiPicker", "MenuListener", { name = "StartWiFiPicker" })  
  UiControl_AddChild(MULTIPLAYER_PAGE, title)
  UiControl_AddChild(CANVAS, MULTIPLAYER_PAGE)
  UiControl_SetVisible(MULTIPLAYER_PAGE, false)
end

function CreateInGameCreditsPage()
  CREDITS_PAGE = Ui_CreateMenu("CreditsPage", "border3", "..", 0, 0, WIDTH, HEIGHT, 1.0, 22.5)
  local title = Ui_CreateText("Credits", "Credits", 20, TITLE_START, 80, TITLE_START + 50, 1.0, 30.0, false) 
  AddResumeButton(CREDITS_PAGE)
  UiControl_AddChild(CREDITS_PAGE, title)
  UiControl_AddChild(CANVAS, CREDITS_PAGE)
  UiControl_SetVisible(CREDITS_PAGE, false)
end

FREEMEM_CONTROL = nil
LOMEM_CONTROL = nil

function UpdateInGameDebugPage()
  local free = GetFreeMemory()
  UiControl_SetText(FREEMEM_CONTROL, ("Free memory " .. tostring(free / 1024.0) .. "K"))
  if LOMEM < 100000000 then
    UiControl_SetText(LOMEM_CONTROL, ("Low memory warning at" .. tostring(LOMEM / 1024.0) .. "K"))
  end
end

function AddZoomLevel(str, number, count)
  local zoom = "Zoom" .. str
  UiControl_AddChild(DEBUG_PAGE, Ui_CreateButton(zoom, "border88_dark", tostring(number), 340, 80 + count * 22, 400, 100 + count * 22, 1.0, 18.75))
  RegisterUiListener(zoom, "MenuListener", { name = zoom })
end

function CreateInGameDebugPage()
  DEBUG_PAGE = Ui_CreateMenu("DebugPage", "border3", "..", 0, 0, WIDTH, HEIGHT, 1.0, 22.5)
  local title = Ui_CreateText("Debug", "Debug", 20, TITLE_START, 80, TITLE_START + 50, 1.0, 30.0, false) 
  AddResumeButton(DEBUG_PAGE)
  UiControl_AddChild(DEBUG_PAGE, title)
  FREEMEM_CONTROL = Ui_CreateText("FreeMem", "", 20,  80, 100, 100, 1.0, 18.75, false)
  LOMEM_CONTROL = Ui_CreateText("LoMem", "", 20, 100, 100, 120, 1.0, 18.75, false)
  
  UiControl_AddChild(DEBUG_PAGE, Ui_CreateButton("MakeRedKey", "border88_dark", "+Red Key", 20, 120, 120, 140, 1.0, 18.75))
  RegisterUiListener("MakeRedKey", "MenuListener", { name = "MakeRedKey" })
  UiControl_AddChild(DEBUG_PAGE, Ui_CreateButton("MakeBlueKey", "border88_dark", "+Blue Key", 20, 150, 120, 170, 1.0, 18.75))
  RegisterUiListener("MakeBlueKey", "MenuListener", { name = "MakeBlueKey" })
  UiControl_AddChild(DEBUG_PAGE, Ui_CreateButton("MakeWhiteKey", "border88_dark", "+White Key", 20, 180, 120, 200, 1.0, 18.75))
  RegisterUiListener("MakeBlueKey", "MenuListener", { name = "MakeWhiteKey" })
  UiControl_AddChild(DEBUG_PAGE, Ui_CreateButton("GoToGenerator", "border88_dark", "+Portal Near Generator", 20, 205, 220, 225, 1.0, 18.75))
  RegisterUiListener("GoToGenerator", "MenuListener", { name = "GoToGenerator" })
  UiControl_AddChild(DEBUG_PAGE, Ui_CreateButton("MakePortalSpell", "border88_dark", "+Portal Spell", 20, 230, 160, 250, 1.0, 18.75))
  RegisterUiListener("MakePortalSpell", "MenuListener", { name = "MakePortalSpell" })
  UiControl_AddChild(DEBUG_PAGE, Ui_CreateButton("MakeTurnSpell", "border88_dark", "+Turn Spell", 20, 250, 160, 270, 1.0, 18.75))
  RegisterUiListener("MakeTurnSpell", "MenuListener", { name = "MakeTurnSpell" })  
  UiControl_AddChild(DEBUG_PAGE, Ui_CreateButton("MakeWhiteKey", "border88_dark", "+Red Key", 20, 270, 120, 290, 1.0, 18.75))
  RegisterUiListener("MakeWhiteKey", "MenuListener", { name = "MakeWhiteKey" })   
  UiControl_AddChild(DEBUG_PAGE, Ui_CreateButton("UpLevel", "border88_dark", "Level +", 220, 80 + 1 * 22, 320, 100 + 1 * 22, 1.0, 18.75))
  RegisterUiListener("UpLevel", "MenuListener", { name = "UpLevel" })
  UiControl_AddChild(DEBUG_PAGE, Ui_CreateButton("DownLevel", "border88_dark", "Level -", 220, 80 + 2 * 22, 320, 100 + 2 * 22, 1.0, 18.75))
  RegisterUiListener("DownLevel", "MenuListener", { name = "DownLevel" })  

  AddZoomLevel('0_25', 0.25, 0)
  AddZoomLevel('0_5',  0.5,  1)
  AddZoomLevel('0_75', 0.75, 2)  
  AddZoomLevel('1_0',  1.0,  3) 
  AddZoomLevel('1_25', 1.25, 4)
  AddZoomLevel('1_5',  1.5,  5)
  AddZoomLevel('2_0',  2.0,  6)    
  
  UiControl_AddChild(DEBUG_PAGE, FREEMEM_CONTROL)
  UiControl_AddChild(DEBUG_PAGE, LOMEM_CONTROL) 
  UiControl_AddChild(CANVAS, DEBUG_PAGE)
  UiControl_SetVisible(DEBUG_PAGE, false)
end
