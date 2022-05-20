set BUILD_SUBDIR="mingw"
set _CMAKE_GEN_="MinGW Makefiles"
call init_project.bat || exit /B 1

mingw32-make Dcm -j5
mingw32-make gtest -j5
mingw32-make DicomTest -j5