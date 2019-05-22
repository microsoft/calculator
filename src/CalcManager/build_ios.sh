#!/bin/bash

xcrun -sdk iphoneos clang \
    -x c++ \
    -arch arm64 \
	-miphoneos-version-min=10.0 \
    -std=c++1z \
    -stdlib=libc++ \
    -c \
	CEngine/*.cpp RatPack/*.cpp *.cpp -I.  

mkdir bin
mkdir bin/arm64

libtool \
    -static \
    *.o \
    -o ../Calculator.iOS/NativeReferences/arm64/libCalcManager.a

rm *.o

clang \
    -x c++ \
    -arch x86_64 \
    -std=c++1z \
    -stdlib=libc++ \
    -c \
	CEngine/*.cpp RatPack/*.cpp *.cpp -I.  

mkdir bin/x86_64

libtool \
    -static \
    *.o \
    -o ../Calculator.iOS/NativeReferences/x86_64/libCalcManager.a

rm *.o

