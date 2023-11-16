@ECHO OFF

IF NOT EXIST Build mkdir Build

pushd Build

SET COMMON_SRC_FILES=..\Code\Vectors.cpp ..\Code\Matrices.cpp ..\Code\Quat.cpp 
SET COMPILER_FLAGS=-MDd -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4244 -wd4100 -wd4189 -wd4456 -wd4457 -wd4701 -wd4324 -wd4239 -wd4505 -FC -Z7
SET LINKER_FLAGS=-incremental:no -opt:ref -NODEFAULTLIB:MSVCRT -LIBPATH:..\External\lib opengl32.lib glfw3.lib user32.lib gdi32.lib shell32.lib -SUBSYSTEM:CONSOLE

del *.pdb > NULL 2> NULL
REM cl %COMPILER_FLAGS% ..\Code\PlatformLayer.cpp %COMMON_SRC_FILES% ..\Code\glad\glad.c -FmPlatformLayer.mp /LD /link -incremental:no -opt:ref -PDB:PlatformLayer%random%.pdb /EXPORT:GameUpdateAndRender
cl %COMPILER_FLAGS% ..\Code\Main.cpp %COMMON_SRC_FILES% ..\Code\glad\glad.c -FmMain.mp /link%LINKER_FLAGS% -PDB:Main%random%.pdb


popd



