#!/bin/bash

pushd . > /dev/null

cd "${0%/*}" # change cwd to script location

yes | cp *.hpp android/jni
yes | cp *.cpp android/jni

cd android

ndk-build

popd > /dev/null
