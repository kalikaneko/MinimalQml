#!/bin/bash
set -e


XBUILD=${XBUILD-no}
WIN64="win64"

TARGET_GOLIB=lib/libgoshim.a


if [ "$XBUILD" == "$WIN64" ]
then
    # TODO allow to override vars
    QMAKE="`pwd`/../mxe/usr/x86_64-w64-mingw32.static/qt5/bin/qmake"
    PATH="`pwd`/../mxe/usr/bin"/:$PATH
    CC=x86_64-w64-mingw32.static-gcc
else
    QMAKE=`which qmake`
fi


function init {
    mkdir -p lib
}

function buildGoLib {
    if [ "$XBUILD" == "no" ]
    then
        echo "Building Go library with standard Go compiler..."
        go build -buildmode=c-archive -o $TARGET_GOLIB go/test.go
    fi
    if [ "$XBUILD" == "$WIN64" ]
    then
        echo "Building Go library with mxe..."
        # TODO need to pass CC?
        CGO_ENABLED=1 GOOS=windows GOARCH=amd64 go build -buildmode=c-archive -o $TARGET_GOLIB go/test.go
    fi
}

function buildQmake {
    $QMAKE -o Makefile minimal.pro
}

echo "[+] Building minimal qml example..."

buildGoLib
buildQmake
make clean
make all
