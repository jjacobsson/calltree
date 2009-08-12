@echo off

y: 
cd calltree
cd

:VS90
if "%VS90COMNTOOLS%" == "" GOTO VS80

:YESVS90
call "%VS90COMNTOOLS%vsvars32.bat"
goto JAM

:VS80
if "%VS80COMNTOOLS%" == "" GOTO NOVS

:YESVS80
call "%VS80COMNTOOLS%vsvars32.bat"
goto JAM

:NOVS
echo You dont seem to have Visual Studio 8 or 9 installed. Cant build.
goto EXIT

:JAM
jam %*

:EXIT