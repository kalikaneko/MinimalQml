#!/bin/sh
set -e
echo "building minimal qml..."
mkdir -p lib
go build -buildmode=c-archive -o lib/libgoshim.a go/test.go
qmake -o Makefile minimal.pro
make all
