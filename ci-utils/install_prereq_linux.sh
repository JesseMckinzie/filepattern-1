#!/bin/bash
# Usage: $bash install_prereq_linux.sh $INSTALL_DIR
# Default $INSTALL_DIR = ./local_install
#
if [ -z "$1" ]
then
      echo "No path to the Filepattern source location provided"
      echo "Creating local_install directory"
      Z5_INSTALL_DIR="local_install"
else
     Z5_INSTALL_DIR=$1
fi

mkdir -p $Z5_INSTALL_DIR
mkdir -p $Z5_INSTALL_DIR/include

git clone https://github.com/pybind/pybind11.git
cd pybind11
mkdir build_man
cd build_man/
cmake -DCMAKE_INSTALL_PREFIX=../../$Z5_INSTALL_DIR/  -DPYBIND11_TEST=OFF ..
make install -j4
cd ../../
