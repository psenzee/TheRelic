print "Lua:dialogtext.lua"

DIALOG_TEXT = 
{
  L1_A_IWait             = { NotRead = "\nI've awaited you..", AlreadyRead = "\nYes?" },
  L1_A_WhoIsPlayer       = "\nHa, it is true you do not know!\t\n\nYou and your brethren are\t\nprisoners here.",
  L1_A_MuchToDo          = "\nThere is much for you to do..\t\t\n\nThat is - if you seek freedom.",  
  L1_A_WhereAmI          = "\nThese are the depths of an ancient\ndungeon called \\#808080Pandora's Mouth\\#ffffff.\t\nEscape is\tunlikely.",
  L1_A_WhoSentMeHere     = "\nWell,\tthe \\#808080Quinotaur\\#ffffff, of course.",
  L1_A_TheQuinotaur      = "\nA man possibly and certainly a\nbeast.\t He rules the living, the dead,\t\nand we who are neither.\t\tHe uses\n\\#808080The Relic\\#ffffff to turn the dead against\nthe quick.",
  L1_A_AosIntro          = "\nI am \\#808080Aos\\#ffffff,\ta watcher.\n\nI will be watching you\tfrom every\ncorner of this pit.",
  L1_A_TheRelic          = "\nIt is \\#808080The Relic\\#ffffff that makes this hole\na hell.",
  L1_A_DestroyGenerator  = "\nTo leave, you must destroy the\n\\#808080Forges of Souls\\#ffffff.\t\n\nThere is one at this depth not far\nfrom here.",
  L1_A_TheGenerator      = "\nThe \\#808080Forges of Souls\\#ffffff could\nbe considered pieces of \\#808080The Relic\\#ffffff.\n",
  L1_A_HowToDestroyIt    = "\nTo reach and destroy the\nforge you must use magic as\nwell as your significant physical\nstrength.",
  L1_A_HowMagic          = "\nSeek the \\#808080Amaranth Ring\\#ffffff, hidden in\na room nearby. \tIt will enable you\nto perform great feats of sorcery.\t\t\n\nWill you retrieve this ring?",
  L1_A_TellMeAbout       = "\nWhat do you wish to know about?\n\nPotions, Spells, Keys or Gold?",
  L1_A_AboutPotions      = "\nPotions:$TODO",
  L1_A_AboutPortals      = "\n.. with thoughtful use, the \\#808080Portal Spell\\#ffffff\ncan make your quests much easier.",
  L1_A_AboutSpells       = "\nSpells:$TODO",
  L1_A_AboutKeys         = "\nKeys:$TODO",
  L1_A_AboutGold         = "\nGold:$TODO",
  L1_A_YesRing           = "\nReturn when you have obtained it.\t\t\n\nTake this key.  You will need it.",
  L1_A_YesGenerator      = "\nWe shall speak again when you've\ndestroyed the forge.",
  L1_A_TimeForGenerator  = "\nNow is the time to destroy the\ngenerator.\t\n\nWill you accept this task?",
  L1_A_DestroyedGenerator = "\nWonderful!  You have destroyed\nthe first of the \\#808080Forges of\nSouls!\\#ffffff  You have given me reason\nto hope.\n\nAscend the stairs and I'll await\nyou there.",

  Relic_Terminal          = "\n\\#808080Model \\#ffffffRELC\\#808080 023 codename \\#c0c0c0'DawnOfDead'\n" ..
                            --"\\#8080ffBiogenerative Weapon Prototype\\#ffffff\n" ..
                            "\\#ffffffAmaranth Systems of Amaranth, Ontario\n" ..
                            "\\#808080Constructed \\#8080ffFebruary 15, 2074\\#808080\n" ..
                            "UN Ontario Underground Testing Facility\n" ..
                            "\\#8080ffAOS\\#808080 Version:\\#ffffff Amaranth Operating System 1.3.7\n" ..
                            "\n\\#c0c0c0Last Biomass Sampled: \\#8080ffNovember 17, 2276\\#808080 00:13:46\n" ..
                            "\\#c0c0c0- Genome:\t\t\\#808080/AS/genomes/22761117.dna\n" ..
                            "\\#c0c0c0- Neural:\t\t\\#808080/AS/neurals/22761117.synapse\n" ..
                            "\n\\#ff4040error(s)! safechecks missi.. (1397563 times; see log)\n" 
}

DIALOG_OPTION_TEXT = 
{
  L1_Q_How               = "How?",
  L1_Q_WhoAmI            = "Who am I?",
  L1_Q_WhoAreYou         = "Who are you?",  
  L1_Q_WhyWait           = "Why do you wait for me?",
  L1_Q_WhereAmI          = "Where am I?",
  L1_Q_WhoSentMeHere     = "Who sent me here?",    
  L1_Q_WhatIsTheRelic    = "What is the Relic?",
  L1_Q_WhoIsTheQuinotaur = "Who is the Quinotaur?",
  L1_Q_WhatToDo          = "What do you expect me to do?",
  L1_Q_WhatIsGenerator   = "What is a Generator of Souls?",
  L1_Q_HowToDestroyIt    = "How do I destroy a Generator?",
  L1_Q_HowMagic          = "Magic? I know nothing of magic!",
  L1_Q_HaveRingWhatNext  = "I have the ring. What next?",  
  L1_Q_TellMeAbout       = "Tell me about..",
  L1_Q_AboutPotions      = "Potions",
  L1_Q_AboutSpells       = "Spells",
  L1_Q_AboutKeys         = "Keys",
  L1_Q_AboutGold         = "Gold",
  
  L1_Q_TurnSpell         = "What is the Turn spell?",  
  L1_Q_PortalSpell       = "What is the Portal spell?",
  L1_Q_UnlockSpell       = "What is the Unlock spell?",  
  
  L1_Q_RedPotion         = "What does a Red potion do?",
  L1_Q_BluePotion        = "What does a Blue potion do?",
  L1_Q_YesRing           = "Yes, I'll find the Amaranth Ring.",
  L1_Q_YesGenerator      = "Yes, I'll destroy the Generator.",
}

DIALOGS = 
{
--  L1_A_IWait             = { "L1_Q_WhoAreYou", "L1_Q_WhereAmI", "L1_Q_WhoAmI", "L1_Q_WhyWait", "L1_Q_TellMeAbout" },
  L1_A_IWait             = { "L1_Q_WhyWait" },
  L1_A_WhoIsPlayer       = { "L1_Q_WhereAmI" },
--  L1_A_WhereAmI          = { "L1_Q_WhoSentMeHere", "L1_Q_WhoAreYou" },
  L1_A_WhereAmI          = { "L1_Q_WhoSentMeHere" },
  L1_A_WhoSentMeHere     = { "L1_Q_WhoIsTheQuinotaur" },
  L1_A_TheQuinotaur      = { "L1_Q_WhatIsTheRelic" },
  L1_A_MuchToDo          = { "L1_Q_WhatToDo" },
--L1_A_DestroyGenerator  = { "L1_Q_WhatIsGenerator", "L1_Q_HowToDestroyIt" },
  L1_A_DestroyGenerator  = { "L1_Q_HowToDestroyIt" },
  L1_A_TheGenerator      = { "L1_Q_WhatIsTheRelic" },
  L1_A_HowToDestroyIt    = { "L1_Q_HowMagic" },
  L1_A_AosIntro          = {},
  L1_A_TheRelic          = {},
  L1_A_HowMagic          = { "L1_Q_YesRing" },
  L1_A_TellMeAbout       = { "L1_Q_AboutPotions", "L1_Q_AboutSpells", "L1_Q_AboutKeys", "L1_Q_AboutGold", },
  L1_A_AboutPotions      = {},
  L1_A_AboutSpells       = {},
  L1_A_AboutKeys         = {},
  L1_A_AboutGold         = {},
  L1_A_YesRing           = {},
  L1_A_YesGenerator      = {},
  L1_A_TimeForGenerator  = { "L1_Q_YesGenerator" },
  L1_A_DestroyedGenerator = {},
}

DIALOG_ANSWERS = 
{
  L1_Q_How               = "",
  L1_Q_WhoAmI            = "L1_A_WhoIsPlayer",
  L1_Q_WhoAreYou         = "L1_A_AosIntro",
  L1_Q_WhatToDo          = "L1_A_DestroyGenerator",
  L1_Q_WhyWait           = "L1_A_MuchToDo",
  L1_Q_WhereAmI          = "L1_A_WhereAmI",
  L1_Q_WhoSentMeHere     = "L1_A_WhoSentMeHere",
  L1_Q_WhatIsTheRelic    = "L1_A_TheRelic",
  L1_Q_WhoIsTheQuinotaur = "L1_A_TheQuinotaur",
  L1_Q_WhatIsGenerator   = "L1_A_TheGenerator",
  L1_Q_HowToDestroyIt    = "L1_A_HowToDestroyIt",
  L1_Q_HowMagic          = "L1_A_HowMagic",
  L1_Q_TellMeAbout       = "L1_A_TellMeAbout",
  L1_Q_AboutPotions      = "L1_A_AboutPotions",
  L1_Q_AboutSpells       = "L1_A_AboutSpells",
  L1_Q_AboutKeys         = "L1_A_AboutKeys",
  L1_Q_AboutGold         = "L1_A_AboutGold",
  L1_Q_YesRing           = "L1_A_YesRing",
  L1_Q_YesGenerator      = "L1_A_YesGenerator",
  L1_Q_HaveRingWhatNext  = "L1_A_TimeForGenerator",
}

LOG_ENTRIES =
{
 "Where am I?  I don't remember..\n\nI must get out!\n",
 "[MP: I have brothers here with me!]\n",
 "The apparition Aos is a Watcher,\na mysterious being I met in\nPandora's Mouth.\n", -- [1]
 "Perhaps he can serve as a guide -\nof sorts.\n\nHe has a strange habit of\nappearing everywhere.\n",

 "The Relic is an ancient artifact\nthat gives birth to the horrors\nof the dungeon.\n",
 "Each level of the dungeon holds a\nGenerator of Souls.\n\nAnd each is a piece of the Relic.\n",
 "The Quinotaur,\na ruler of great evil,\nuses the Relic to summon the dead\nand subdue the living.\n",
 "To free myself from this hell,\nAos says I must defeat\nthe Quinotaur and\ndestroy the Relic.\n",

 "I found an ancient artifact,\nthe Amaranth Ring.\n\nI am able to command magic!\n", -- [1: The Cave of Amaranth]
 "I defeated a\nGenerator of Souls!\n\nI fear that more await..", -- [1: The Inferior Generator]

 "Aos is trapped,\nimprisoned in his role\nby the Quinotaur.\n", -- [2]

 "I defeated the Skeleton King!\n", -- [3: The Hall of the Skeleton King] (and/or Arachnarex)
 
 "I defeated Arachnarex,\nthe Giant Spider!\n", -- [4: Arachnarex's Lair]

 "I found the \\#808080Book of The Before\\#ffffff\nwhich I summarize -\n", -- [4]

 "Long ago, the world ended,\nthen began again.\n\n" .. 
 "Then one thousand years ago,\nservants of a minor lord discovered\na cave entrance deep in the forest.\n",
 
 "Inside, this lord found the Relic,\nan ancient device of\nunfathomable power.\n",
 "It was not active or alive,\nbut over time this lord\nreenchanted it.\n",
 "He called the cave Pandora's Mouth\nwhen he saw the\nhorrors that emerged.\n",
 "The gods of \\#808080The Time Before\\#ffffff\nhad great mastery of magics.\n\nGreater than we can comprehend.\n",
 "They have left behind artifacts\nof great power -\n\nOur spellbooks, rings, and of course,\nthe Relic.\n",

-- Aos clarifies [5]

 "The Relic was and \\#808080is\\#ffffff a weapon.\n\nCreated not to defeat death, but to\nturn evil life against good.\n",
 "The \\#808080gods of before\\#ffffff were not gods,\nbut men.\n\nAs men, they mastered\nthe creation of life.\n",
 "And turned the mastery of life\nto the mastery of others,\n\nand turned the creation of life\ninto an instrument of death.\n",
 --"The magic of the world before produced incredibly destructive weapons.\n",
 --"Yet their world was ultimately destroyed by the Relic.\n",
 "Of the countless weapons\nthey created, ultimately it was\nthe Relic that destroyed them.\n",
 --"The Relic is a weapon, more destructive than any the world had ever known.\n",
 "It was through the Relic\nthat the world before was\ndestroyed..\n\nAnd Aos played a crucial role!\n",

-- The Quinotaur [6]

 "I now face the Quinotaur..\n",

 "The Quinotaur was not the\nlord that initially\nentered Pandora's Mouth!\n",
 "The blood of that early lord\npowered the artifact.\n\nThe Quinotaur is one of many\ncopies the Relic made of\nthat lord over time.\n",
 "..and so am I!!\n",
 "I defeated the Quinotaur!\n", --  [6: The Quinotaur's Chamber]

 "These regions have strange names like\n\\#808080Ontario Underground Testing Facility,\n\\#808080Amaranth Systems\\#ffffff and\n\\#808080Geothermal Power Facility\\#ffffff\n",
 "Glowing markings appear on\nthe Relic..\n",
 
 "-RELIC-TEXT-",
 
 "I defeated the Relic!\n", --  [9: The Relic]
 "I see the exit above..\n",
-- "I am free!!\n",
 
 "-END-"

}
