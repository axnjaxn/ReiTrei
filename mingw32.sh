#!/bin/sh

# Use this: ./configure --host i686-w64-mingw32 --target i686-w64-mingw32 --prefix=/usr/i686-w64-mingw32

PREFIX=i686-w64-mingw32
export MINGW=mingw
export CC=$PREFIX-gcc
export CXX=$PREFIX-g++
export CPP=$PREFIX-cpp
export PATH="/usr/$PREFIX/bin:$PATH"
exec "$@"