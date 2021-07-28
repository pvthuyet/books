@echo off
break>testdata.txt
setlocal EnableDelayedExpansion
for /l %%x in (1,1,10) do (
	set "formattedValue=000000%%x"
	set v=!formattedValue:~-3!
	echo hello %v% >> testdata.txt
)