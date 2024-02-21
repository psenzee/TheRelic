copy C:\Dungeon3dArt\Final\ballR*.obj C:\Dungeon3dArt\ex\
for %%x in (C:\Dungeon3dArt\ex\ballR*.obj) do meshtool -transform %%x %%x -scale 3.0 9.0 3.0
copy C:\Dungeon3dArt\ex\ballR*.obj C:\root\d4\d0814_2\_configs\
ipmesh_relic ballR*.obj
copy ballR*.ips meshes
bundler meshes/*.ips meshes.bin
copy meshes.bin c:\Relic\trunk\data\meshes
rem pause
