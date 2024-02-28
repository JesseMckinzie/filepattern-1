#!/bin/bash -e
# Usage: $bash install_prereq_linux.sh $INSTALL_DIR
# Default $INSTALL_DIR = ./local_install
#
if [ -z "$1" ]
then
      echo "No path to the install location provided"
      echo "Creating local_install directory"
      LOCAL_INSTALL_DIR="local_install"
else
     LOCAL_INSTALL_DIR=$1
fi

mkdir -p "$LOCAL_INSTALL_DIR"
mkdir -p "$LOCAL_INSTALL_DIR"/include

curl -L https://github.com/pybind/pybind11/archive/refs/tags/v2.9.2.zip -o v2.9.2.zip
unzip v2.9.2.zip
cd pybind11-2.9.2
mkdir build_man
cd build_man/
cmake -DCMAKE_INSTALL_PREFIX=../../"$LOCAL_INSTALL_DIR"/  -DPYBIND11_TEST=OFF ..
make install -j4
cd ../../
