REM rmdir /s /q build
REM mkdir build
cd build
cmake -G "MinGW Makefiles" ..\src\
mingw32-make -j4
move smashpp.exe ..
cd ..