#!/bin/bash

if [[ ! -n "$TOP_DIR" ]]; then
  TOP_DIR=$(cd `dirname $0`; pwd)
fi

case $1 in
 "android")
   mkdir -p ../out/android
   cd ../out/android
   cmake -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_BUILD_PLATFORM=android \
         -DCMAKE_TOOLCHAIN_FILE=$TOP_DIR/android.toolchain.cmake \
         -DANDROID_NDK=$ANDROID_NDK \
         -DANDROID=TRUE \
         ../..
   make
   rm ../../flint/jni/flint-jni/libs/armeabi/libflint-android.so
   cp flint/libflint.so ../../flint/jni/flint-jni/libs/armeabi/libflint-android.so
   ;;

"ffos")
  mkdir -p ../out/ffos
  cd ../out/ffos
  cmake -DCMAKE_C_COMPILER=`which ccache` \
        -DCMAKE_C_COMPILER_ARG1=arm-linux-androideabi-gcc  \
        -DCMAKE_CXX_COMPILER=`which ccache` \
        -DCMAKE_CXX_COMPILER_ARG1=arm-linux-androideabi-g++  \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_BUILD_PLATFORM=ffos \
        -DFFOS=TRUE \
        ../..
  make
  ;;

 # soundkit
 "soundkit")
   mkdir -p ../out/soundkit
   cd ../out/soundkit
   cmake -DCMAKE_C_COMPILER=`which ccache` \
         -DCMAKE_C_COMPILER_ARG1=mipsel-linux-gnu-gcc \
         -DCMAKE_CXX_COMPILER=`which ccache` \
         -DCMAKE_CXX_COMPILER_ARG1=mipsel-linux-gnu-g++ \
         -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_BUILD_PLATFORM=linux \
         -DCMAKE_BUILD_PRODUCT=soundkit \
         ../..
   make
   ;;

"linux")
  mkdir -p ../out/linux
  cd ../out/linux
  cmake -DCMAKE_C_COMPILER=`which ccache` \
        -DCMAKE_C_COMPILER_ARG1=gcc \
        -DCMAKE_CXX_COMPILER=`which ccache` \
        -DCMAKE_CXX_COMPILER_ARG1=g++ \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_BUILD_PLATFORM=linux \
        ../..
  make
  ;;

"mac")
  mkdir -p ../out/mac
  cd ../out/mac
  cmake -DCMAKE_C_COMPILER=`which gcc-4.8` \
        -DCMAKE_CXX_COMPILER=`which g++-4.8` \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_BUILD_PLATFORM=mac \
        ../..
  make
  ;;
*)
  echo Invalid devices to build
  ;;
esac
