@echo off
:: set CC=clang-cl.exe  -Wno-microsoft -Wall -Werror -Wfatal-errors
set CC=c:\TDM-GCC-32\bin\gcc.exe -std=c11 -g 
:: -Wall -Werror -Wfatal-errors -I..
set GDB=c:\TDM-GCC-32\bin\gdb32.exe  -ex run
:: echo %~dp0
%~d3
CD %3
set ROOT=%CD%


echo ----------- run lex1 ... ----------
%ROOT%\expr.exe %ROOT%\expr-test.lc -o expr-test.c
IF NOT %ERRORLEVEL% EQU 0 GOTO error_l
echo ''
echo --------- test successful ---------

:: cmd.exe /k %ROOT%\calc.exe
:: IF %ERRORLEVEL% EQU 0 GOTO test2_l
:: echo RUNTIME ERROR (%ERRORLEVEL%)
:: %GDB% %ROOT%\test\lex1.exe  %ROOT%\test\src2.c
:: goto end_l
::  
:: :test2_l
:: echo ----------- run lex2 ... ----------
:: %ROOT%\test\lex2.exe %ROOT%\test\src2.c
:: IF %ERRORLEVEL% EQU 0 GOTO end_l
:: echo RUNTIME ERROR (%ERRORLEVEL%)
:: %GDB% %ROOT%\test\lex2.exe  %ROOT%\test\src2.c
GOTO end_l

:error_l



:end_l
:: lua