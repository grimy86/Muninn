#include <MuninnDal.h>

DWORD WINAPI TestThread(HMODULE hModule)
{
    MessageBoxW(NULL, L"Hello from the DLL!", L"Test DLL", MB_OK);
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
        HANDLE hThread{ CreateThread(
                    nullptr,
                    0,
                    (LPTHREAD_START_ROUTINE)TestThread,
                    hModule,
                    0,
                    nullptr) };

        DAL_CloseHandle32(hThread);
        break; 
    }

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}