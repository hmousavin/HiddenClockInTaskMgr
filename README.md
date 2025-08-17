# Hidden Clock In Task Manager

This project demonstrates how to:
- Build a .exe (Clock.exe) that will be used as a hidden app.
- Build an injector program (Main.exe) that injects HideClock.dll into the taskmgr.exe process.

## Requirements
- Windows (tested on Windows 10+)
- [CMake](https://cmake.org/) 3.15 or newer
- Visual Studio 2022 (or another MSVC-based compiler)

## Building
```bash
git clone git@github.com:hmousavin/HiddenClockInTaskMgr.git
cd HiddenClockInTaskMgr
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

This will produce the following binaries inside build/Release/:
- build/Release/Main.exe
- build/Release/HideMe.dll
- build/Release/Clock.exe

## Usage
- The injector will automatically injects the HideClock.dll into taskmgr.exe to hide the "Clock.exe" before executing that.

## Important
After building the project, you must open the taskmgr in your machine, before using this.