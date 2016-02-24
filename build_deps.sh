#!/bin/sh

set -e
set -x

cd deps/file/
autoreconf -ivf
./configure --enable-static
make clean
make
make check
