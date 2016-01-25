#!/bin/sh

set -e
set -x

cd deps/file/
autoreconf -ivf
./configure --enable-static
make
make check
