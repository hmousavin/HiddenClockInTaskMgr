# HideMe Explorer

This project demonstrates how to:
- Build a DLL (Payload.dll) that will be injected.
- Build an injector program (Injector.exe) that injects Payload.dll into the explorer.exe process.

⚠ **Disclaimer:**  
This code is strictly for educational and authorized testing purposes only. Do not inject into processes you do not own or have permission to modify. Unauthorized injection can cause instability and may violate laws or software licenses.

## Requirements
- Windows (tested on Windows 10+)
- [CMake](https://cmake.org/) 3.15 or newer
- Visual Studio 2022 (or another MSVC-based compiler)

## Building
```bash
git clone https://github.com/yourusername/hideme-explorer.git
cd hideme-explorer
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

This will produce the following binaries inside build/Release/:
- build/Release/Injector.exe
- build/Release/Payload.dll

## Usage
- The injector will automatically locate explorer.exe, inject Payload.dll from the current working directory, and print progress messages.

## Important
After injection, you will not see the .hideme file because the injection hides it.
To reset the explorer shell and restore normal visibility, you must restart explorer.exe by ending it and starting it again (e.g., via Task Manager or taskkill /f /im explorer.exe followed by start explorer.exe).