#!/bin/bash

pushd . > /dev/null

cd "${0%/*}" # change cwd to script location

rm -rf dist-win32
mkdir dist-win32
cd dist-win32

cp -r ../dist/assets .
perl -pi -e 's/\n/\r\n/' assets/*.txt
perl -pi -e 's/\n/\r\n/' assets/levels/*

i686-w64-mingw32-cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
make -j 4

find . -path '*[M|m]ake*' -delete

MINGW_BUNDLEDLLS_SEARCH_PATH=/usr/i686-w64-mingw32/bin mingw-bundledlls --copy space-logic-adventure.exe # https://github.com/mpreisler/mingw-bundledlls

popd > /dev/null
