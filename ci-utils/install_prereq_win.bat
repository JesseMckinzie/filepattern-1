mkdir local_install
mkdir local_install\include

git clone https://github.com/pybind/pybind11.git
pushd pybind11
mkdir build_man
pushd build_man
cmake -DCMAKE_INSTALL_PREFIX=../../local_install/  -DPYBIND11_TEST=OFF ..
cmake --build . --config Release --target install  
popd
popd


if errorlevel 1 exit 1

if "%ON_GITHUB%"=="TRUE" xcopy /E /I /y local_install\bin %TEMP%\filepattern\bin