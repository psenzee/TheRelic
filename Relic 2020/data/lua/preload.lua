print "Lua:preload.lua"

PreloadTextures =
{
    'blue_glow_full',
    'shadow',
    'barbarian01',
    'axe',
    'barbarian01_hurt',
    'tick_new_2',
    'black_button_potion',
    'black_button_sword',
    'black_button_ring',
--    'skeleton_05_ds',
    'skelweapon_01small',
    'skel_shield_dead',
    'skeleton_03_hurt',
--    'skeleton_03',
    'skel_dead',
--    'skeleton_07_ds',
--    'skeleton_02',
--    'skeleton_08_ds',
--    'skeleton_04_ds',
--    'skeleton_09_ds',
--    'skeleton_10_ds',
    'blue_glow',
    'Necro1_hurt',
    'Necro2',
    'Spider0_hurt',
--    'Spider1',
--    'Spider2',
--    'Spider3',
--    'Spider4',
--    'Spider5',
--    'GeneratorGhost2',
    'Barrel',
    'BrokenBarrel2',
    'goldpieces5',
    'keySq128White',
    'keySq128Red',
    'keyButton128W',
    'keyButton128R',
    'keyButton128B',
--    'keySq128Green',
--    'keySq128Yellow',
    'keySq128Blue',
    'ShadowPotion',
    'Book11',
    'Book10',
    'Book9',
    'Book8',
    'Book7',
    'PotionRed',
    'PotionBlue',
    'PotionGreen',
    'PotionYellow',
    'PotionPurple',
    'PotionOrange',
    'PotionCyan',
    'PotionWhite',
    'PotionBlack',
    'SeekerBewareWhite',
    'DoorLockPlain',
    'DoorSide1',
    'DoorSide2',
    'Top2',
--    'FloorStone3_14',
    'Black',
--    'Baked_UpStairsL_9',
    'Shadows',
--    'Baked_DownStairs_9',
--    'StoneGrassFloor3',
--    'StoneGrassFloor4',
--    'Floor_Ice',
--    'Floor_IceAlt',
    'Shadow_Crease',
    'Shadow_TopOutCorner',
--    'WallTypesB_9',
--    'WallTypesA_9',
    'Shadow_TopInCorner',
    'LavaColumn0',
    'Shadow_TopWall',
--    'OriginalArch',
    'Crate3',
    'MarketSmall',
    'Shadow_Market',
    'TentSmall',
    'Shadow_Tent',
    'TentRed',
    'Shadow_Machine2',
    'Shadow_Machine1',
    'Machine12',
    'health_bar_7',
    'health_small_shadow2',
--    'ring12',
    'blueringsword',
    'magic2_opaq',
    'blueringfirepotion_opaq',
    'bluering',
    'stick2',
    'stick2_hilite',
--  'pause2',
    'dust_glow_shadow_desat',
--    'ring11',
    'blueringsword_hilite',
    'blue_glow_shadow',  -- PNG
    'CircleEffect8', -- PNG
    'canvas',
    
    'intro10c',
    'intro11c',
    'intro11d',
}

PRELOAD_TEXTURE_COUNT = #PreloadTextures
LOAD_TEXTURES_PER_FRAME = 10

function PreloadTextureSet()
  if PRELOAD_TEXTURE_COUNT <= 0 then
    return true
  end
  -- preload textures, max LOAD_TEXTURES_PER_FRAME per frame
  for i = 1, LOAD_TEXTURES_PER_FRAME do
    if PRELOAD_TEXTURE_COUNT > 0 then
      PreloadTexture(PreloadTextures[PRELOAD_TEXTURE_COUNT])      
      PRELOAD_TEXTURE_COUNT = PRELOAD_TEXTURE_COUNT - 1
    end
  end
  return PRELOAD_TEXTURE_COUNT <= 0
end

MUSIC_THEME = nil
MUSIC_ENABLED = true

function StopMusicTheme()
  if MUSIC_ENABLED then
	MUSIC_THEME = nil
	Audio_StopTheme()
  end
end

function PlayMusicTheme(name)
  if MUSIC_ENABLED and MUSIC_THEME ~= name then
    MUSIC_THEME = name
    if IsIPhone() then
      Audio_PlayTheme(name .. ".wav.mp4", 0.5)
    else
      Audio_PlayTheme(name .. ".wav", 0.5)
    end
  end
end

function StartAudio()
--Audio_Load("bricksfall.wav", "Fall")
  Audio_Load("sword_big_clang02.wav", "SwordOnSword")
  Audio_Load("bricksfall.wav", "Pickup")
  Audio_Load("sword_strike_body_slash_04.wav", "SkeletonKill")
  Audio_Load("zombie_flesh_beast_pain_01.wav", "SpiderKill")
  Audio_Load("crate_smash_02.wav", "BarrelKill")    
  Audio_Load("hiss_beast_screech_01.wav", "Hiss1")
  Audio_Load("dooropen.wav", "Door")
  Audio_Load("implosion2.wav", "Explode")
  Audio_Load("electricalcurrent.wav", "Electricity")
  Audio_Load("hiss.wav", "Hiss2")
  Audio_Load("magic.wav", "Portal")  
  Audio_Load("steamburst_sauna01.wav", "Magic")
--Audio_Load("sword_whoosh01.wav", "PlayerWhoosh")
  Audio_Load("swing.wav", "PlayerWhoosh")
--Audio_Load("footstep_single_dirt_01_var01.wav", "SkeletonHit")    
  Audio_Load("kick.wav", "SkeletonHit")        
  Audio_Load("footstep_single_dirt_01_var01.wav", "Step")
--Audio_PlayTheme("return_of_the_knights_loop.wav", 0.5)
  PlayMusicTheme("Relic")
  Audio_SetAmbientVolume(0.5)
--Audio_StartAmbient()
end

LOAD_STATE_NONE             = 0
LOAD_STATE_MESHES           = 1
LOAD_STATE_TEXTURES         = 2
LOAD_STATE_RENDERABLES      = 3
LOAD_STATE_GAME_STATE       = 4
LOAD_STATE_WAIT             = 10
LOAD_STATE_AUDIO            = 11
LOAD_STATE_MULTIPLAYER      = 12
LOAD_STATE_STARTED          = 13

LOAD_STATE                  = LOAD_STATE_NONE

function CanUpdateMultiplayer()
  return LOAD_STATE >= LOAD_STATE_MULTIPLAYER
end

function LogLoadStateIn()
  print(string.format("\nPreloadAndStart loading state in: %d ", LOAD_STATE))
end

function SetLoadState(state)
  LOAD_STATE = state
  print(string.format("\nPreloadAndStart loading state out: %d ", LOAD_STATE))
  if LOAD_STATE >= LOAD_STATE_STARTED then
    return true
  end
  return false
end

-- returns true if done, false if not
function PreloadAndStart()
  if LOAD_STATE >= LOAD_STATE_STARTED then
    return true
  end
  LogLoadStateIn()
  if LOAD_STATE == LOAD_STATE_NONE then
    LOCK_INPUT = true
    SetPaused(true)
    return SetLoadState(LOAD_STATE_MESHES)
  elseif LOAD_STATE == LOAD_STATE_MESHES then
    LOCK_INPUT = true
    AddContentBundle("meshes.bin")
    return SetLoadState(LOAD_STATE_TEXTURES)
  elseif LOAD_STATE == LOAD_STATE_TEXTURES then
    LOCK_INPUT = true
    state = LOAD_STATE
    if PreloadTextureSet() then
      state = LOAD_STATE_RENDERABLES
    end
    return SetLoadState(state)
  elseif LOAD_STATE == LOAD_STATE_RENDERABLES then
    LOCK_INPUT = true
    LoadRenderables()
    return SetLoadState(LOAD_STATE_GAME_STATE)
  elseif LOAD_STATE == LOAD_STATE_GAME_STATE then
    LOCK_INPUT = true
    StartGameState()
    return SetLoadState(LOAD_STATE + 1)
  elseif LOAD_STATE < LOAD_STATE_WAIT then -- delay for Start()
    LOCK_INPUT = true
    return SetLoadState(LOAD_STATE + 1)
  elseif LOAD_STATE == LOAD_STATE_WAIT then
    LOCK_INPUT = true
    return SetLoadState(LOAD_STATE_AUDIO)
  elseif LOAD_STATE == LOAD_STATE_AUDIO then
    LOCK_INPUT = true
    SetPaused(false)
    StartAudio()
    return SetLoadState(LOAD_STATE_MULTIPLAYER)
  elseif LOAD_STATE == LOAD_STATE_MULTIPLAYER and IS_JOINING then
    LOCK_INPUT = true
    SetLoadingMessage("Joining")
    return SetLoadState(LOAD_STATE_MULTIPLAYER)
  elseif LOAD_STATE == LOAD_STATE_MULTIPLAYER then
    LOCK_INPUT = false
    RestoreLoadingMessage()
    return SetLoadState(LOAD_STATE_STARTED)
  end
  return true
end
