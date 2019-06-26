#!/bin/bash

CommonFlags="-DDEBUG -g -Og -Wall -Werror -Wno-write-strings -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-sign-compare -Wno-unused-result -Wno-strict-aliasing -Wno-switch -std=gnu++11 -fno-rtti -fno-exceptions -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_SDL=1"

CODE_PATH="$(dirname "$0")"
mkdir -p "$CODE_PATH/../hhbuild"
pushd "$CODE_PATH/../hhbuild"

# Build asset file builder
#c++ $CommonFlags ../cpp/code/test_asset_builder.cpp -o test_asset_builder

# Build a 64-bit version
c++ $CommonFlags -O2 ../handmade/handmade_optimized.cpp -c -fPIC -o handmade_optimized.o
c++ $CommonFlags ../handmade/handmade.cpp handmade_optimized.o -fPIC -shared -o handmade.so.temp
mv handmade.so.temp handmade.so

c++ $CommonFlags ../handmade/sdl_handmade.cpp -o handmadehero.x86_64 -ldl `../handmade/sdl2-64/bin/sdl2-config --cflags --libs` -Wl,-rpath,'$ORIGIN/x86_64'

# Build a 32-bit version
#c++ -m32 $CommonFlags ../handmade/handmade.cpp -fPIC -shared -o handmade.so
#c++ -m32 $CommonFlags ../handmade/sdl_handmade.cpp -o handmadehero.x86 -ldl `../handmade/sdl2-32/bin/sdl2-config --cflags --libs` -Wl,-rpath,'$ORIGIN/x86'

#Copy SDL into the right directory.
if [ ! -d "x86_64" ]
then
  mkdir -p x86_64
  cp ../handmade/sdl2-64/lib/libSDL2-2.0.so.0 x86_64/
fi
if [ ! -d "x86" ] 
then
  mkdir -p x86
  cp ../handmade/sdl2-32/lib/libSDL2-2.0.so.0 x86/
fi
popd

