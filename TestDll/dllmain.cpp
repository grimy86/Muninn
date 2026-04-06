#include <MuninnDal.h>

DWORD WINAPI TestThread(HMODULE libModule)
{
    MessageBoxW(NULL, L"Hello from the DLL!", L"Test DLL", MB_OK);
    
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