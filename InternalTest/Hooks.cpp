#include "Offsets.h"
#include "Hooks.h"
#include <cstdio>
#include <MuninnDal.h>

void InstallHook(HookInfo* hInfo)
{
    if (!hInfo || !hInfo->targetAddress || !hInfo->size)
    {
        printf("InstallHook: invalid HookInfo.\n");
        return;
    }

	// Save original bytes if not already saved
    if (!hInfo->originalBytes)
    {
        hInfo->originalBytes = new BYTE[hInfo->size];
        memcpy(hInfo->originalBytes,
            hInfo->targetAddress,
            hInfo->size);
        hInfo->ownsOriginalBytes = TRUE;
    }

    switch (hInfo->type)
    {
    case HookType::Patch:
        DAL_PatchMemory32(
            hInfo->targetAddress,
            hInfo->patchBytes,
            hInfo->size);
        break;

    case HookType::Trampoline:
        DAL_WriteRelativeTrampoline32(
            hInfo->targetAddress,
            hInfo->hookAddress,
            hInfo->size,
            &hInfo->gateway);
        break;

    case HookType::Detour:
        DAL_WriteRelativeDetour32(
            hInfo->targetAddress,
            hInfo->hookAddress,
            hInfo->size);
        break;

    default:
        printf("InstallHook: unknown HookType.\n");
        break;
    }
}

void UninstallHook(HookInfo* hInfo)
{
    if (!hInfo || !hInfo->targetAddress || !hInfo->size)
    {
        printf("UninstallHook: invalid HookInfo.\n");
        return;
    }

    switch (hInfo->type)
    {
    case HookType::Trampoline:
        DAL_RestoreRelativeTrampoline32(
            hInfo->targetAddress,
            hInfo->gateway,
            hInfo->size);
        break;

    case HookType::Patch:
    case HookType::Detour:
        if (!hInfo->originalBytes)
        {
            printf("UninstallHook: originalBytes is nullptr, cannot restore.\n");
            return;
        }

        DAL_PatchMemory32(
            hInfo->targetAddress,
            hInfo->originalBytes,
            hInfo->size);

        if (hInfo->ownsOriginalBytes)
        {
            delete[] hInfo->originalBytes;
            hInfo->originalBytes = nullptr;
            hInfo->ownsOriginalBytes = FALSE;
        }
        break;

    default:
        printf("UninstallHook: unknown HookType.\n");
        break;
    }
}

int __cdecl CEScanHook()
{
	static int count = 0;
	int gameTimer = *(int*)(offsets::AssaultCube + offsets::gameTimer);
	printf("CEScanHook: count[%i], gameTimer[%i]\r", ++count, gameTimer);
	return gameTimer;
}

int __cdecl CEScanTrampolineHook()
{
    int t = *(int*)(offsets::AssaultCube + offsets::gameTimer);
    return printf("CEScanTrampolineHook called @ %i\r", t),
        ((CEScan_t)ceScanTrampolineHookInfo.gateway)();
}

void __declspec(naked) RecoilAssemblyHook()
{
	__asm
	{
		push eax
		mov eax, 0x50F4F4
		mov eax, [eax]              // localPlayer
		mov eax, [eax + 0x374]      // weaponPtr
		mov eax, [eax + 0x14]       // ammoPtr
		mov[eax], 0x539             // set ammo to 1337
		pop eax

		mov word ptr[edi + 0x122], 0x0
		movsx ecx, word ptr[edi + 0x122]

		jmp dword ptr[recoilJumpBackAddress]
	}
}