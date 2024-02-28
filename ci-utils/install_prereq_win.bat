mkdir local_install
mkdir local_install\include

curl -L https://github.com/pybind/pybind11/archive/refs/tags/v2.10.0.zip -o v2.10.0.zip
unzip v2.10.0.zip
pushd pybind11-2.10.0
mkdir build_man
pushd build_man
cmake -DCMAKE_INSTALL_PREFIX=../../local_install/  -DPYBIND11_TEST=OFF ..
cmake --build . --config Release --target install
popd
popd


if errorlevel 1 exit 1
