@echo OFF
echo start subscribers...
for /L %%x in (1,1,3) do (
start "subscriber" syncsub.exe
)
echo start publisher...
start "publisher" syncpub.exe