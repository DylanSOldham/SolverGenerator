setlocal
SET PATH=%PATH%;%cd%\mingw64\bin
".build\generator.exe" system.txt
cd .build
make solver -j4
cd ..
".build\solver.exe" 1> out.csv