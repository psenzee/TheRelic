copy C:\Dungeon3dArt\ex_*.* C:\Dungeon3dArt\ex\
copy C:\Dungeon3dArt\collide_*.* C:\Dungeon3dArt\ex\
copy C:\Dungeon3dArt\Door*.* C:\Dungeon3dArt\ex\
for %%x in (C:\Dungeon3dArt\ex\ex_*.obj) do meshtool -transform %%x %%x -rotatex 90.0
for %%x in (C:\Dungeon3dArt\ex\collide_*.obj) do meshtool -transform %%x %%x -rotatex 90.0
for %%x in (C:\Dungeon3dArt\ex\ex_*.obj) do meshtool -transform %%x %%x -scale 42.66667 42.66667 42.66667
for %%x in (C:\Dungeon3dArt\ex\collide_*.obj) do meshtool -transform %%x %%x -scale 42.66667 42.66667 42.66667
for %%x in (C:\Dungeon3dArt\ex\ex_*.obj) do meshtool -transform %%x %%x -translate 0 0 10.0
for %%x in (C:\Dungeon3dArt\ex\ex_*.obj) do meshtool -unseam %%x %%x
for %%x in (C:\Dungeon3dArt\ex\Door*.obj) do meshtool -transform %%x %%x -scale 42.66667 42.66667 42.66667
rem for %%x in (C:\Dungeon3dArt\ex\collide_*.obj) do meshtool -transform %%x %%x -translate 0 0 10.0
copy C:\Dungeon3dArt\ex\ex_*.* C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\collide_*.* C:\root\d4\d0814_2\_configs\
copy C:\Dungeon3dArt\ex\Door*.* C:\root\d4\d0814_2\_configs\
rem pause
