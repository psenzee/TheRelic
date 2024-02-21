rem for %%x in (*.obj) do stripe -wq %%x
rem for %%x in (*.stripe) do ipmesh %%x
rem ipmesh *.stripe
ipmesh *.obj
rem make a map
tobin test.txt test.emp
