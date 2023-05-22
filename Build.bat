@ECHO OFF

if not exist Build mkdir Build

pushd Build

set PROGRAM=Memory
set SRC=..\Code\Main.cpp
set COMPILE_FLAGS=-FC -Zi
set LINK_FLAGS=user32.lib gdi32.lib shell32.lib -SUBSYSTEM:CONSOLE

cl %COMPILE_FLAGS% -Fe%PROGRAM% %SRC% -link%LINK_FLAGS%
popd