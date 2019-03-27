set BUILD_DIR="msvc2017_x32_clang"
set _CMAKE_GEN_="Visual Studio 15"
call init_project.batcmake.exe ..\.. -G "Visual Studio 15 2017" -T "LLVM-vs2014"
