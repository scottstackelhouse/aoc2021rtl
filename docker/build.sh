#!/bin/bash -e
# DESCRIPTION: Build Verilator (inside container)
#
## Original copyright:
## Copyright 2020 by Stefan Wallentowitz. This program is free
## software; you can redistribute it and/or modify it under the terms
## of the GNU Lesser General Public License Version 3.

# Portions Copyright 2021 by Scott Stackelhouse.  You can redistribute
# and/or modify this modified version under the terms of the GNU Lesser
# General Public License Version 3
#
# Modifications consist are related to rebasing on debian.  The original
# can be found at https://github.com/wallento/verilator.

: "${REPO:=https://github.com/verilator/verilator}"

# cocotb supports only verilator 4.106 for now.
# https://github.com/verilator/verilator/issues/2778
# https://github.com/cocotb/cocotb/issues/2300
: "${REV:=v4.106}"
: "${CC:=gcc}"
: "${CXX:=g++}"

GCCVERSION=$(${CC} --version | grep gcc | awk '{print $4}')

export SYSTEMC_INCLUDE="/opt/systemc/include"
export SYSTEMC_LIBDIR="/opt/systemc/lib"
export LD_LIBRARY_PATH=${SYSTEMC_LIBDIR}

SRCS=$PWD/verilator

git clone "$REPO" "$SRCS"
cd "$SRCS"
git checkout "$REV"
autoconf
./configure
make
if [ "${1:-''}" == "test" ]; then
    make test
fi
