@echo OFF
IF exist "build" (
ECHO Deleting CMake artifacts...
rd /s /q build
)
mkdir build
cd build
ECHO Running CMake...
cmake -G "Visual Studio 15 2017 Win64" ..
pause