mkdir project
mkdir project\msvs2017_clang

cd project\msvs2017_clang

set PATH=%PATH%;C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin

cmake.exe ..\.. -G "Visual Studio 15 2017" -T "LLVM-vs2014"
