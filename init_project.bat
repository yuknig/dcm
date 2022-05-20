git submodule update --init --recursive

set BUILD_DIR=project\%BUILD_SUBDIR%
if not exist %BUILD_DIR% mkdir %BUILD_DIR% || exit /B 1
cd %BUILD_DIR% || exit /B 1

set _CMAKE_ARGS_=-Dgtest_force_shared_crt=ON
cmake.exe ..\.. -G %_CMAKE_GEN_% %_CMAKE_ARGS_% || exit /B 1
