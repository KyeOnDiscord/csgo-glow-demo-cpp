#pragma once
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcID(const wchar_t* ProcessName);

uintptr_t GetModuleBaseAddress(DWORD ProccesssID, const wchar_t* ModuleName);

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);