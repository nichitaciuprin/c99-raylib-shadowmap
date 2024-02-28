@echo off
setlocal

set root=%~f0\..\..\..

set mingw=C:\Nichita\projects\BoidsRaylib\deps\mingw\x86_64-12.2.0-release-posix-seh-ucrt-rt_v10-rev2\mingw64\bin
@REM set mingw=%root%\deps\mingw\x86_64-13.2.0-release-posix-seh-ucrt-rt_v11-rev0\mingw64\bin
@REM set mingw=C:\Users\User1\Downloads\mingw\x86_64-13.2.0-release-posix-seh-ucrt-rt_v11-rev0\mingw64\bin
set path=%mingw%

gdb -q -ex=run -ex=backtrace -ex=quit --args build\main.exe

@REM build\main.exe

@REM if %errorlevel% neq 0 echo === ERROR === %errorlevel%