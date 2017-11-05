@echo off
:: set CC=clang-cl.exe  -Wno-microsoft -Wall -Werror -Wfatal-errors
set CC=c:\TDM-GCC-32\bin\gcc.exe -std=c11 -g 
:: -Wall -Werror -Wfatal-errors -I..
set GDB=c:\TDM-GCC-32\bin\gdb32.exe  -ex run
:: echo %~dp0
%~d3
CD %3
set ROOT=%CD%


%CC% -Wno-unused-function -Wall %ROOT%\lc.c %ROOT%\ast.c -o %ROOT%\lc.exe
IF NOT %ERRORLEVEL% EQU 0 GOTO end_l

echo ----------- run lex1 ... ----------
%ROOT%\lc.exe %ROOT%\lc-test.lc
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

:end_l
:: echo ALL TESTS SUSSESS
:: lua