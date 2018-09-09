mkdir project
mkdir project\msvs2017

cd project\msvs2017

cmake.exe ..\.. -G "Visual Studio 15 2017 Win64" -Dgtest_force_shared_crt=ON