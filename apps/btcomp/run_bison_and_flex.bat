@ECHO OFF
echo.

if "%1" == "build" goto build_lbl
if "%1" == "clean" goto clean_lbl

echo You need to pass either "build" or "clean" as the first argument
goto exit_error_lbl

:build_lbl

cd source/parser

echo bison source/parser/parser.y
bison parser.y
echo flex source/parser/lexer.l
flex lexer.l

cd ../..

if not errorlevel 0 goto exit_error

goto exit_good_lbl

:clean_lbl

echo Cleaning source\parser.c source\parser.h source\lexer.c source\parser.output 

cd source/parser

del /F /Q parser.c
del /F /Q parser.h
del /F /Q parser.output
del /F /Q lexer.c
del /F /Q lexer.h

goto exit_good_lbl

:exit_error_lbl
echo.
echo Finished with errors
exit /b 1

:exit_good_lbl
echo.
echo Finished successfully
exit /b 0