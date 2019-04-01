set BUILD_DIR="mingw"
set _CMAKE_GEN_="MinGW Makefiles"
call init_project.bat

mingw32-make Dcm -j5
mingw32-make gtest -j5
mingw32-make DicomTest -j5