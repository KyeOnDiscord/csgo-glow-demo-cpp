#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <time.h>
#include <ctime>


#include "includes/json.hpp"


#include "offsets.h"
#include "proc.h"
#include "KyeMem.h"
#include "GlowStruct.h"
#include "ChamStruct.h"
using namespace nlohmann;

#define Print(x) std::cout << x << std::endl
#define Sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x));
#define Ptr uintptr_t
#define Offset uintptr_t


#define Netvar(x) offsets["netvars"][x]
#define Signature(x) offsets["signatures"][x]




int main()
{
    SetConsoleTitle(TEXT("CSGO External Glow Hack | Made by Kye#5000"));
    system("Color A");
	json offsets = GetcsgoOffsets();//Downloads offsets from hazedumper
    Print("Fetched offsets from hazedumper");
	DWORD procId = GetProcID(L"csgo.exe");
	if (procId == 0)
	{
		Print("CSGO is not opened!");
        std::cin.get();
        return 0;
	}
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
	KyeMem Memory(hProcess);
    Print("Opened Process csgo.exe (" << procId << ")");

	Ptr clientdll = GetModuleBaseAddress(procId, L"client.dll");
	Ptr enginedll = GetModuleBaseAddress(procId, L"engine.dll");



	Ptr LocalPlayer = Memory.ReadProcMemory<int>(clientdll + Signature("dwLocalPlayer"));
	Ptr clientstate = Memory.ReadProcMemory<int>(enginedll + Signature("dwClientState"));

    
	while (true)
	{
		int MaxPlayerCount = Memory.ReadProcMemory<int>(clientstate + Signature("dwClientState_MaxPlayer"));

		//Basically sleep the hack if ur in menu and such
		if (MaxPlayerCount < 1)
		{
			Sleep(500);
			continue;
		}

		//Get our glow object
		int glowObject = Memory.ReadProcMemory<int>(clientdll + Signature("dwGlowObjectManager"));
		//Get our team number
		int myTeam = Memory.ReadProcMemory<int>(LocalPlayer + Netvar("m_iTeamNum"));


        for (int i = 0; i < MaxPlayerCount; i++)
        {
            //The current entity
            int entity = Memory.ReadProcMemory<int>(clientdll + Signature("dwEntityList") + i * 0x10);
            bool bDormant = Memory.ReadProcMemory<bool>(entity + Signature("m_bDormant"));

            if (!bDormant)
            {
                int glowIndex = Memory.ReadProcMemory<int>(entity + Netvar("m_iGlowIndex"));
                int entityTeam = Memory.ReadProcMemory<int>(entity + Netvar("m_iTeamNum"));
                Offset CurrentEntityGlowOffset = glowObject + (glowIndex * 0x38);
                GlowStruct currentGlow = Memory.ReadProcMemory<GlowStruct>(CurrentEntityGlowOffset);
                ChamStruct currentCham = {};

                //These variables will be the same for any of the teams
                currentGlow.Alpha = 1.7f;
                currentGlow.renderOccluded = true;
                currentGlow.renderUnoccluded = false;

                   if (myTeam == entityTeam)
                   {
                       currentGlow.Red = 0.0f;
                       currentGlow.Green = 1.0f;
                       currentGlow.Blue = 0.0f;
                   }
                   else
                   {
                       currentGlow.Red = 1.0f;
                       currentGlow.Green = 0.0f;
                       currentGlow.Blue = 0.0f;
                       if (Memory.ReadProcMemory<bool>(entity + Netvar("m_bIsDefusing")))
                       {
                           currentGlow.Red = 255.0f;
                           currentGlow.Green = 255.0f;
                           currentGlow.Blue = 255.0f;
                       }
                       //Our teammates are shown on map so we only have to write the radar to our enemies. (Basically this: https://youtu.be/5VOkRJk1GVg)
                       Memory.WriteProcMemory<bool>(entity + Netvar("m_bSpotted"), true);
                   }

                   //Actually write the glow
                   Memory.WriteProcMemory<GlowStruct>(CurrentEntityGlowOffset, currentGlow);

                   //Set the cham colors to what the glow is
                   //*255 idea from: https://stackoverflow.com/a/46575472/12897035
                   currentCham.Red = (UINT8)round(currentGlow.Red * 255);
                   currentCham.Green = (UINT8)round(currentGlow.Green * 255);
                   currentCham.Blue = (UINT8)round(currentGlow.Blue * 255);
                   //Actually write the cham
                   Memory.WriteProcMemory<ChamStruct>(entity + Netvar("m_clrRender"), currentCham);
               }
            }
        Sleep(1);
        }
}