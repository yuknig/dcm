mkdir project
mkdir project\mingw

cd project\mingw

cmake.exe ..\.. -G "MinGW Makefiles"

mingw32-make Dcm -j5
mingw32-make gtest -j5
mingw32-make DicomTest -j5