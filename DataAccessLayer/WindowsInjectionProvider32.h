#ifndef WINDOWS_INJECTION_PROVIDER_32_H
#define WINDOWS_INJECTION_PROVIDER_32_H

#include "DalConfig.h"

#ifndef NTSTATUS
#define NTSTATUS LONG
#endif // !NTSTATUS

MUNINN_API NTSTATUS MUNINN_CALL
DAL_SimpleDLLInjectA32(
	_In_ const HANDLE processHandle,
	_In_ const CHAR* const dllPath,
	_Out_ HMODULE* pModuleHandle);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_SimpleDLLInjectW32(
	_In_ const HANDLE processHandle,
	_In_ const WCHAR* const dllPath,
	_Out_ HMODULE* pModuleHandle);

#endif // !WINDOWS_INJECTION_PROVIDER_32_H