#include <windows.h>
#include <cstdio>
#include "Globals.h"

void __declspec(naked) LaserGunAsmHook()
{
	__asm
	{
		push eax
		mov eax, 0x50F4F4
		mov eax, [eax]              // localPlayer
		mov eax, [eax + 0x374]      // weaponPtr
		mov eax, [eax + 0x14]       // ammoPtr
		mov[eax], 0x15             // set ammo to 21
		pop eax

		mov word ptr[edi + 0x122], 0x0
		movsx ecx, word ptr[edi + 0x122]
		jmp AssaultCube::recoilJumpBackAddress // 0x46227B
	}
}

int __cdecl ScanForCheatEngineHook()
{
	printf("ScanForCheatEngineHook called @ %i\n", *(int*)AssaultCube::gameTimer);
	return *(int*)AssaultCube::gameTimer;
}

int __cdecl ScanForCheatEngineTrampolineHook()
{
	bool doGatewayCall{ true };
	printf("ScanForCheatEngineTrampolineHook called @ %i\n", *(int*)AssaultCube::gameTimer);

	if (doGatewayCall)
	{
		printf("Gateway called. \n");
		return ((AssaultCube::ScanForCheatEngine_t)AssaultCube::gateway)();
	}
	
	printf("Gateway not called. \n");
	return 0;
}