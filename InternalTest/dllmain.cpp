#include <MuninnDal.h>
#include "Offsets.h"
#include "Hook.h"

const char* iconFilePath{ "C:\\Program Files (x86)\\AssaultCube\\packages\\crosshairs\\cube.png" };

DWORD WINAPI TestThread(HMODULE libModule)
{
    MessageBoxW(NULL, L"Hello from the DLL!", L"Test DLL", MB_OK);
    
    AssaultCube::SDL_WM_SetCaption("Hello from the DLL!", nullptr);
	AssaultCube::SDL_Surface* iconSurface{ AssaultCube::IMG_Load(iconFilePath) };

	if (iconSurface == nullptr)
    {
        MessageBoxW(NULL, L"Failed to load icon.png", L"Error", MB_OK | MB_ICONERROR);
        FreeLibraryAndExitThread(libModule, 1);
        return 1;
    }

	AssaultCube::SDL_WM_SetIcon(iconSurface, nullptr);


	PatchMemory(AssaultCube::shotDelay, AssaultCube::shotDelayPatch, sizeof(AssaultCube::shotDelayPatch));
	PatchMemory(AssaultCube::kickBackMultiplyer, AssaultCube::kickBackMultiplyerPatch, sizeof(AssaultCube::kickBackMultiplyerPatch));

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