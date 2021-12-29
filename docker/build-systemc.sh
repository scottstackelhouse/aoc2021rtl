#!/bin/bash -e
# DESCRIPTION: Build SystemC with different g++/gcc
#
## Original copyright notice:
## Copyright 2020 by Stefan Wallentowitz. This program is free
## software; you can redistribute it and/or modify it under the terms
## of the GNU Lesser General Public License Version 3.

# Portions Copyright 2021 by Scott Stackelhouse.  You can redistribute
# and/or modify this modified version under the terms of the GNU Lesser
# General Public License Version 3
#
# Modifications consist are related to rebasing on debian and reducing the number
# of versions built.  The original can be found at https://github.com/wallento/verilator.

wget https://www.accellera.org/images/downloads/standards/systemc/systemc-2.3.3.tar.gz
tar -xzf systemc-2.3.3.tar.gz
cd systemc-2.3.3
mkdir build
cd build
cmake ../ -D ENABLE_PTHREADS=ON -D CMAKE_CXX_STANDARD=14
make -j $(nproc)
make -j $(nproc) check && \
make install 
cd ..
#rm -r build
cd ..
#rm -rf systemc-2.3.3
rm systemc-2.3.3.tar.gz


