@echo off
setlocal

set root=%~f0\..\..

set mingw=C:\Nichita\projects\BoidsRaylib\deps\mingw\x86_64-12.2.0-release-posix-seh-ucrt-rt_v10-rev2\mingw64\bin
@REM set mingw=%root%\deps\mingw\x86_64-13.2.0-release-posix-seh-ucrt-rt_v11-rev0\mingw64\bin
@REM set mingw=C:\Users\User1\Downloads\mingw\x86_64-13.2.0-release-posix-seh-ucrt-rt_v11-rev0\mingw64\bin
set path=%mingw%

@REM -s -mwindows
set flags=-g3 -O0 -Wall -std=c99 -Wno-missing-braces -pthread -m64
@REM set flags=-g3 -O3 -Wall -std=c99 -Wno-missing-braces -pthread -m64

set dep1=-I%root%\libs\include -L%root%\libs\static -lraylib -lglmf32 -lm -lopengl32 -lgdi32 -lwinmm -ldloadhelper
set deps=%dep1% %dep2% %dep3% %dep4%

if not exist build mkdir build

@REM g++ --version
gcc main.c -o build/main.exe %flags% %deps%
@REM g++ -c main.cpp -o build/main.exe %flags% %deps%