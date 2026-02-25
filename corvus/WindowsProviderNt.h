#pragma once
#include "WindowsStructures.h"

namespace Corvus::Data
{
	HANDLE OpenProcessHandleNt(const DWORD processId, const ACCESS_MASK accessMask);
	BOOL CloseProcessHandleNt(HANDLE handle);
	PROCESS_EXTENDED_BASIC_INFORMATION GetProcessInformation(HANDLE hProcess);
	std::wstring GetImageFileNameNt(HANDLE hProcess);
	std::wstring GetImageFileNameWin32Nt(HANDLE hProcess);


	DWORD GetQSIBufferSizeNt(const SYSTEM_INFORMATION_CLASS sInfoClass);
	std::wstring GetRemoteUnicodeStringNt(HANDLE hProcess, const UNICODE_STRING& unicodeString);

	uintptr_t GetModuleBaseAddressNt(DWORD processId, const std::wstring& processName);
	Corvus::Object::ArchitectureType GetArchitectureTypeNt(HANDLE hProcess);
	std::wstring GetObjectNameNt(HANDLE hObject, DWORD processId);
	std::wstring GetObjectTypeNameNt(HANDLE hObject, DWORD processId);
	BOOL GetModuleInformation(Corvus::Object::ProcessEntry& processEntry);
	BOOL GetThreadInformation(Corvus::Object::ProcessEntry& processEntry);
	BOOL GetHandleInformation(Corvus::Object::ProcessEntry& processEntry);

	template <typename T>
	NTSTATUS SetVirtualMemoryNt(HANDLE hProc, uintptr_t baseAddress, const T& value)
	{
		return NtWriteVirtualMemory(
			hProc,
			reinterpret_cast<PVOID>(baseAddress),
			&value,
			sizeof(T),
			nullptr);
	}

	template <typename T>
	NTSTATUS GetVirtualMemoryNt(HANDLE hProc, uintptr_t baseAddress, T& out)
	{
		return NtReadVirtualMemory(
			hProc,
			reinterpret_cast<PVOID>(baseAddress),
			&out,
			sizeof(T),
			nullptr);
	}
}