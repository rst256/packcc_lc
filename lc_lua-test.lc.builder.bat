@echo off
:: set CC=clang-cl.exe  -Wno-microsoft -Wall -Werror -Wfatal-errors
set CC=c:\TDM-GCC-32\bin\gcc.exe -std=c11 -g 
:: -Wall -Werror -Wfatal-errors -I..
set GDB=c:\TDM-GCC-32\bin\gdb32.exe  -ex run
:: echo %~dp0
%~d3
CD %3



echo ----------- Runing ... ------------
%CD%\lc_lua.exe %CD%\lc_lua-test.lc -i %CD%\lc_lua_h.lua -o lc_lua-test.c