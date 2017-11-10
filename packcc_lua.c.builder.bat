@echo off
:: set CC=clang-cl.exe  -Wno-microsoft -Wall -Werror -Wfatal-errors
set CC=c:\TDM-GCC-32\bin\gcc.exe -std=c11 -g 
:: -Wall -Werror -Wfatal-errors -I..
set GDB=c:\TDM-GCC-32\bin\gdb32.exe  -ex run
:: echo %~dp0
%~d3
CD %3

%CC% %CD%\packcc_lua.c  -g3 -std=gnu99 -Bstatic -IC:\Projects\Lua\cmodules\lua53\src    -IC:\Projects\Lua\cmodules     -o packcc_lua.exe C:\Projects\Lua\cmodules\lua53\src\lua53.dll


echo ---------- Generating ... ----------
packcc_lua -o lc_lua  %CD%\lc_lua.cc 
:: IF NOT %ERRORLEVEL% EQU 0 GOTO error_l
:: echo successful

