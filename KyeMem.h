#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>
class KyeMem
{
public:
	KyeMem() {};
	HANDLE CurrentProcess = new HANDLE();
	KyeMem(HANDLE hProcess)
	{
		CurrentProcess = hProcess;
	}
	template <typename T>
	void WriteProcMemory(uintptr_t Address, T value)
	{
		WriteProcessMemory(CurrentProcess, (BYTE*)Address, &value, sizeof(value), 0);
	}

	template <typename T>
	T ReadProcMemory(uintptr_t Address)
	{
		T val;
		ReadProcessMemory(CurrentProcess, (BYTE*)Address, &val, sizeof(T), 0);
		return val;
	}



	//FindDMAAddy
	uintptr_t FindPtrVal(uintptr_t addr, std::vector<unsigned int> offsets)
	{
		for (UINT i = 0; i < offsets.size(); i++)
		{
			ReadProcessMemory(CurrentProcess, (BYTE*)addr, &addr, sizeof(addr), 0);
			addr += offsets[i];
		}
		return addr;
	}
};