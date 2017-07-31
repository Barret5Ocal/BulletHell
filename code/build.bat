@echo off

IF NOT EXIST ..\..\build mkdir ..\..\build

pushd ..\..\build

set LIBS=User32.lib Winmm.lib Gdi32.lib opengl32.lib 

cl -nologo -Zi ..\project\code\win32_bullethell.cpp %LIBS%

popd 