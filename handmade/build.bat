@echo off

set CommonCompilerFlags=-MTd -nologo -Gm- -GR- -EHa -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib

IF NOT EXIST ..\hhbuild mkdir ..\hhbuild
pushd ..\hhbuild

del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% ..\handmade\handmade.cpp -Fmhandmade.map -LD /link -incremental:no -opt:ref -PDB:handmade_%random%.pdb -EXPORT:GameGetSoundSamples -EXPORT:GameUpdateAndRender
cl %CommonCompilerFlags% ..\handmade\sdl_handmade.cpp -Fmsdl_handmade.map /link %CommonLinkerFlags%
popd