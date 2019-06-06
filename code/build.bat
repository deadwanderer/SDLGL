@echo off

set CommonCompilerFlags= /std:c++17 -MTd -nologo -Gm- -GR- -EHa- -EHsc- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4996
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib opengl32.lib gdi32.lib winmm.lib ..\lib\SDL2.lib ..\lib\SDL2main.lib ..\lib\assimp.lib

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% ..\code\main.cpp ..\code\glad.c ..\include\imgui.cpp ..\include\imgui_impl_sdl_gl3.cpp ..\include\imgui_demo.cpp ..\include\imgui_draw.cpp /link %CommonLinkerFlags%
popd