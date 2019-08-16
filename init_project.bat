git submodule update --init --recursive

call set_path.bat

mkdir project\%BUILD_DIR%
cd project\%BUILD_DIR%

set _CMAKE_ARGS_=-DBOOST_DIR=%BOOST_DIR%; -Dgtest_force_shared_crt=ON
cmake.exe ..\.. -G %_CMAKE_GEN_% %_CMAKE_ARGS_%
