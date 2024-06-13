
IF not exist mingw64.zip (
    curl.exe -o mingw64.zip -L https://github.com/brechtsanders/winlibs_mingw/releases/download/14.1.0mcf-11.0.1-ucrt-r1/winlibs-x86_64-mcf-seh-gcc-14.1.0-mingw-w64ucrt-11.0.1-r1.zip
)
IF not exist mingw64 (
    tar.exe -xf mingw64.zip
)

setlocal
SET PATH=%PATH%;%cd%\mingw64\bin
ren "%cd%\mingw64\bin\mingw32-make.exe" "make.exe"

%cd%\mingw64\bin\cmake.exe -Wno-dev -G "Unix Makefiles" -B .build . -DCMAKE_PROGRAM_PATH=%cd%\mingw64\bin

cd .build
%cd%\..\mingw64\bin\make.exe -j4
cd ..