#include <iostream>
#include <map>
#include <vector>
#include "KeInterface.h"
#pragma comment(lib,"ntdll.lib")
#include <TlHelp32.h>
#include <algorithm>

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR FullPathName[256];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

using namespace std;

void Shoot()
{
	mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, NULL, NULL);
	Sleep(10);
	mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, NULL, NULL);
}

int main()
{

	KeInterface Driver("\\\\.\\kernelhop");


	SetConsoleTitle(L"TEST");
	// Get address of client.dll & pid of csgo from our driver
	DWORD ProcessId = Driver.GetTargetPid();
	DWORD ClientAddress = Driver.GetClientModule();

	// Get address of localplayer
	DWORD LocalPlayer = Driver.ReadVirtualMemory<DWORD>(ProcessId, ClientAddress + LOCAL_PLAYER, sizeof(ULONG));

	// address of inground
	DWORD InGround = Driver.ReadVirtualMemory<DWORD>(ProcessId,
		LocalPlayer + FFLAGS, sizeof(ULONG));

	
	while (true)
	{
		
		int crossId = Driver.ReadVirtualMemory<int>(ProcessId, LocalPlayer + CrosshairID, sizeof(int));
		int LocalTeam = Driver.ReadVirtualMemory<int>(ProcessId, LocalPlayer + teamNum, sizeof(int));
		if (crossId > 0 && crossId < 64 && (GetAsyncKeyState(VK_MENU) & 0x8000))
		{
			if (LocalTeam != Driver.ReadVirtualMemory<int>(ProcessId, LocalPlayer + teamNum * crossId, sizeof(int)))
			{
				Shoot();
			}
		}

		// No Flash
		Driver.WriteVirtualMemory(ProcessId, LocalPlayer + FlashMaxAlpha, 0.f, 8);

		// Bunny Hop
		Driver.WriteVirtualMemory(ProcessId, ClientAddress + FORCE_JUMP, 0x5, 8);
		DWORD InGround = Driver.ReadVirtualMemory<DWORD>(ProcessId, LocalPlayer + FFLAGS, sizeof(ULONG));
		if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && (InGround & 1 == 1))
		{
			// Jump
			Driver.WriteVirtualMemory(ProcessId, ClientAddress + FORCE_JUMP, 0x5, 8);
			Sleep(50);
			// Restore
			Driver.WriteVirtualMemory(ProcessId, ClientAddress + FORCE_JUMP, 0x4, 8);
			
		}
		Sleep(10);
	}
    return 0;
}

