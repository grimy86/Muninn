#include <MuninnDal.h>
#include "Globals.h"
#include "HookUtilities.h"
#include "Hooks.h"

const char* iconFilePath{ "C:\\Program Files (x86)\\AssaultCube\\packages\\crosshairs\\cube.png" };

DWORD WINAPI TestThread(HMODULE libModule)
{
    // Allocate console for debug output
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    // Set window caption and icon
    AssaultCube::SDL_WM_SetCaption("Hello from the DLL!", nullptr);
    AssaultCube::SDL_Surface* iconSurface{ AssaultCube::IMG_Load(iconFilePath) };

    if (iconSurface == nullptr)
    {
        printf("Failed to load icon.png\n");
        FreeLibraryAndExitThread(libModule, 1);
        return 1;
    }

    AssaultCube::SDL_WM_SetIcon(iconSurface, nullptr);

    // Apply memory patches
    PatchInstructions(
        AssaultCube::shotDelayInstruction,
        AssaultCube::shotDelayPatch,
        sizeof(AssaultCube::shotDelayPatch));

    PatchInstructions(
        AssaultCube::kickBackMultiplyer,
        AssaultCube::kickBackMultiplyerPatch,
        sizeof(AssaultCube::kickBackMultiplyerPatch));

    InstallHook(
        AssaultCube::recoilInstruction,
        (uintptr_t)LaserGunAsmHook,
        AssaultCube::recoilInstructionLength);

    /*
    InstallHook(
        AssaultCube::scanForCheatEngineFunction,
        (uintptr_t)ScanForCheatEngineHook,
        6);
    */

    InstallTrampolineHook(
        AssaultCube::scanForCheatEngineFunction,
        (uintptr_t)ScanForCheatEngineTrampolineHook,
		6);

    // Keep DLL alive until END key is pressed
    while (!GetAsyncKeyState(VK_END))
    {
        Sleep(100);
    }

    FreeLibraryAndExitThread(libModule, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);

        HANDLE hThread{ CreateThread(
                    nullptr,
                    0,
                    (LPTHREAD_START_ROUTINE)TestThread,
                    hModule,
                    0,
                    nullptr) };

        if (hThread)
            CloseHandle(hThread);

        break;
    }

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}