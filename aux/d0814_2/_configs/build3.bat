copy C:\Dungeon3dArt\ex_*.obj C:\Dungeon3dArt\ex\
copy C:\Dungeon3dArt\Final\ex_*.obj C:\Dungeon3dArt\ex\
copy C:\Dungeon3dArt\collide_*.obj C:\Dungeon3dArt\ex\
copy C:\Dungeon3dArt\Door*.obj C:\Dungeon3dArt\ex\
copy C:\Dungeon3dArt\Final\anim*.obj C:\Dungeon3dArt\ex\
copy C:\Dungeon3dArt\Final\Barrel*.obj C:\Dungeon3dArt\ex\
copy C:\Dungeon3dArt\Final\ballR*.obj C:\Dungeon3dArt\ex\
for %%x in (C:\Dungeon3dArt\ex\anim_barb_idle_body_0000*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
for %%x in (C:\Dungeon3dArt\ex\anim_barb_idle_axe_0000*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
for %%x in (C:\Dungeon3dArt\ex\anim_barb_run2_0000*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
for %%x in (C:\Dungeon3dArt\ex\anim_barb_run2_body_0000*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
rem for %%x in (C:\Dungeon3dArt\ex\anim_barb_attack_new1_body_0000*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
rem for %%x in (C:\Dungeon3dArt\ex\anim_barb_attack_new1_axe_0000*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
rem for %%x in (C:\Dungeon3dArt\ex\anim_barb_run_attack1_body_0000*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
rem for %%x in (C:\Dungeon3dArt\ex\anim_barb_run_attack1_axe_0000*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
for %%x in (C:\Dungeon3dArt\ex\anim_barb_run2_axe_0000*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
for %%x in (C:\Dungeon3dArt\ex\anim_barb_attack_*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
for %%x in (C:\Dungeon3dArt\ex\anim_barb_attack6_*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
for %%x in (C:\Dungeon3dArt\ex\anim_barb_walk_0000*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
for %%x in (C:\Dungeon3dArt\ex\anim_skel*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
for %%x in (C:\Dungeon3dArt\ex\anim_spider*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
for %%x in (C:\Dungeon3dArt\ex\anim_generatorghost*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
for %%x in (C:\Dungeon3dArt\ex\animateBarbarianAxe__0000*.obj) do meshtool -transform %%x %%x -scale 10.0 10.0 10.0
for %%x in (C:\Dungeon3dArt\ex\Barrel*.obj) do meshtool -transform %%x %%x -scale 15.0 15.0 15.0
for %%x in (C:\Dungeon3dArt\ex\ballR*.obj) do meshtool -transform %%x %%x -scale 15.0 15.0 15.0
for %%x in (C:\Dungeon3dArt\ex\ex_*.obj) do meshtool -transform %%x %%x -rotatex 90.0
for %%x in (C:\Dungeon3dArt\ex\collide_*.obj) do meshtool -transform %%x %%x -rotatex 90.0
for %%x in (C:\Dungeon3dArt\ex\ex_*.obj) do meshtool -transform %%x %%x -scale 42.66667 42.66667 42.66667
for %%x in (C:\Dungeon3dArt\ex\collide_*.obj) do meshtool -transform %%x %%x -scale 42.66667 42.66667 42.66667
for %%x in (C:\Dungeon3dArt\ex\ex_*.obj) do meshtool -transform %%x %%x -translate 0 0 10.0
for %%x in (C:\Dungeon3dArt\ex\ex_*.obj) do meshtool -unseam %%x %%x
rem for %%x in (C:\Dungeon3dArt\ex\Door*.obj) do meshtool -transform %%x %%x -scale 42.66667 42.66667 42.66667
for %%x in (C:\Dungeon3dArt\ex\Door*.obj) do meshtool -transform %%x %%x -scale 42.66667 64.0 42.66667
rem for %%x in (C:\Dungeon3dArt\ex\collide_*.obj) do meshtool -transform %%x %%x -translate 0 0 10.0
copy C:\Dungeon3dArt\ex\Barrel*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\ballR*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\ex_*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\collide_*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\Door*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\animateBarbarianAxe__0000*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_barb_idle_body_0000*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_barb_idle_axe_0000*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_barb_attack_new1_body_0000*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_barb_attack_new1_axe_0000*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_barb_run2_0000*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_barb_run2_body_0000*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_barb_run2_axe_0000*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_barb_run_attack1_body_0000*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_barb_run_attack1_axe_0000*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_barb_walk_0000*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_barb_attack_*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_barb_attack6_*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_skel*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_spider*.obj C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\anim_generatorghost*.obj C:\root\d4\d0814_2\_configs\
rem pause
