#include "proc.h"



DWORD GetProcID(const wchar_t* ProcessName)
{
	DWORD procID = 0;
	HANDLE hSnap = (CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);
		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, ProcessName))
				{
					procID = procEntry.th32ProcessID;
					break;
				}


			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procID;
}


uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* ModuleName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = (CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId));

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);

		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, ModuleName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}

			} while (Module32Next(hSnap, &modEntry));
		}
	}

	CloseHandle(hSnap);
	return modBaseAddr;

}


//Find Dynamic Memory Allocation
uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); i++)
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}