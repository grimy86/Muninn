#pragma once
#include "apiconfig.h"
#include "WindowsStructures.h"

namespace Corvus::Data
{
	template <typename T>
	NTSTATUS WriteVirtualMemoryNt(const HANDLE processHandle, const uintptr_t baseAddress, const T& value)
	{
		return NtWriteVirtualMemory(
			processHandle,
			reinterpret_cast<PVOID>(baseAddress),
			&value,
			sizeof(T),
			nullptr);
	}

	template <typename T>
	NTSTATUS ReadVirtualMemoryNt(const HANDLE processHandle, const uintptr_t baseAddress, T& out)
	{
		return NtReadVirtualMemory(
			processHandle,
			reinterpret_cast<PVOID>(baseAddress),
			&out,
			sizeof(T),
			nullptr);
	}

#pragma region public API
	CORVUS_API NTSTATUS CORVUS_CALL
		OpenProcessHandleNt(
			_In_ const DWORD processId,
			_In_ const ACCESS_MASK accessMask,
			_Out_ HANDLE* const pHandle);

	CORVUS_API NTSTATUS CORVUS_CALL
		CloseHandleNt(_In_ const HANDLE handle);

	/// <summary>
	/// Handles are per-process, duplicating allows us to safely query objects from another process.
	/// </summary>
	/// <param name="sourceHandle"> The source handle to duplicate.
	/// This value is meaningful in the context of the source process. </param>
	/// <param name="processId"> The unique process identifier. </param>
	/// <param name="pDuplicatedHandle"> The duplicated handle buffer. </param>
	/// <returns> A duplicated handle from the source process. </returns>
	CORVUS_API NTSTATUS CORVUS_CALL
		DuplicateHandleNt(
			_In_ const HANDLE sourceHandle,
			_In_ const DWORD processId,
			_Out_ HANDLE* const pDuplicatedHandle) noexcept;

	/// <param name="processHandle"> A handle to the process. </param>
	/// <param name="accessMask"> The desired handle access mask. </param>
	/// <returns> A handle to the acess token of the process. </returns>
	CORVUS_API HANDLE CORVUS_CALL OpenProcessTokenHandleNt(const HANDLE hProcess, const ACCESS_MASK accessMask);


	CORVUS_API uint64_t CORVUS_CALL GetFullLuidNt(const LUID& luid);

	/// <param name="infoClass"> One of the values enumerated in SYSTEM_INFORMATION_CLASS,
	/// which indicates the kind of system information to be retrieved. </param>
	/// <param name="pRequiredBufferSize"> A pointer to the buffer. </param>
	/// <returns> The required buffer size for a NtQuerySystemInformation() call. </returns>
	CORVUS_API NTSTATUS CORVUS_CALL
		GetQSIBufferSizeNt(
			_In_ const SYSTEM_INFORMATION_CLASS infoClass,
			_Out_ DWORD* const pRequiredBufferSize) noexcept;

	/// <param name="duplicatedHandle"> A kernel handle reference to query information about.
	/// The handle does not need to grant any specific access. </param>
	/// <param name="infoClass"> One of the values enumerated in OBJECT_INFORMATION_CLASS,
	/// which indicates the kind of object information to be retrieved. </param>
	/// <param name="pRequiredBufferSize"> A pointer to the buffer. </param>
	/// <returns> The required buffer size for a NtQueryObject() call. </returns>
	CORVUS_API NTSTATUS CORVUS_CALL
		GetQOBufferSizeNt(
			_In_ const HANDLE duplicatedHandle,
			_In_ const OBJECT_INFORMATION_CLASS infoClass,
			_Out_ DWORD* const pRequiredBufferSize) noexcept;

	/// <param name="tokenHandle"> A handle to the process's access token. </param>
	/// <param name="infoClass"> One of the values enumerated in _TOKEN_INFORMATION_CLASS,
	/// which indicates the kind of token information to be retrieved. </param>
	/// <param name="pRequiredBufferSize"> A pointer to the buffer. </param>
	/// <returns> The required buffer size for a NtQueryObject() call. </returns>
	CORVUS_API NTSTATUS CORVUS_CALL
		GetQITBufferSizeNt(
			_In_ const HANDLE tokenHandle,
			_In_ const _TOKEN_INFORMATION_CLASS infoClass,
			_Out_ DWORD* const requiredBufferSize) noexcept;


	CORVUS_API std::wstring CORVUS_CALL GetObjectNameNt(const HANDLE sourceHandle, const DWORD processId);
	CORVUS_API std::wstring CORVUS_CALL GetObjectTypeNameNt(const HANDLE sourceHandle, const DWORD processId);
	CORVUS_API std::wstring CORVUS_CALL GetRemoteUnicodeStringNt(
		const HANDLE processHandle,
		const UNICODE_STRING& unicodeString);

	CORVUS_API PROCESS_EXTENDED_BASIC_INFORMATION CORVUS_CALL GetProcessInformationNt(const HANDLE processHandle);
	CORVUS_API BOOL CORVUS_CALL GetProcessInformationObjectNt(const HANDLE processHandle, Corvus::Object::ProcessEntry& processEntry);

	/// <summary>
	/// Assigns extended native process information to a process entry object reference.
	/// </summary>
	/// <param name="processHandle"> A handle to the process. </param>
	/// <param name="processId"> The unique process identifier. </param>
	/// <param name="processEntry"> A reference to the process entry object. </param>
	/// <returns> TRUE if all values are sucessfully assigned. </returns>
	CORVUS_API BOOL CORVUS_CALL GetProcessInformationObjectExtendedNt(
		const HANDLE processHandle,
		const DWORD processId,
		Corvus::Object::ProcessEntry& processEntry);

	CORVUS_API std::wstring CORVUS_CALL GetImageFileNameNt(const HANDLE processHandle);
	CORVUS_API std::wstring CORVUS_CALL GetImageFileNameWin32Nt(const HANDLE processHandle);

	/// <param name="processHandle"> A handle to the process. </param>
	/// <returns> The PEB base address. </returns>
	CORVUS_API uintptr_t CORVUS_CALL GetPebBaseAddressNt(const HANDLE processHandle);

	/// <param name="processInfo"> A reference to the process information. </param>
	/// <returns> The PEB base address. </returns>
	CORVUS_API uintptr_t CORVUS_CALL GetPebBaseAddressNt(
		const PROCESS_EXTENDED_BASIC_INFORMATION& processInfo);

	/// <summary>
	/// Gets the PEB base address and initializes processInfo reference.
	/// </summary>
	/// <param name="processHandle"> A handle to the process. </param>
	/// <param name="processInfo"> A reference to the process information. </param>
	/// <returns> The PEB base address. </returns>
	CORVUS_API uintptr_t CORVUS_CALL GetPebBaseAddressNt(
		const HANDLE processHandle,
		PROCESS_EXTENDED_BASIC_INFORMATION& processInfo);

	/// <param name="processHandle"> A handle to the process. </param>
	/// <returns> The PEB structure. </returns>
	CORVUS_API PEB CORVUS_CALL GetPebNt(const HANDLE processHandle);

	/// <summary>
	/// Gets the PEB structure and initializes the PEB base address reference.
	/// </summary>
	/// <param name="processHandle"> A handle to the process. </param>
	/// <param name="pebBaseAddress"> A reference to the PEB base address. </param>
	/// <returns> The PEB structure. </returns>
	CORVUS_API PEB CORVUS_CALL GetPebNt(
		const HANDLE processHandle,
		uintptr_t& pebBaseAddress);

	/// <summary>
	/// Internally calls the GetPebBaseAddressNt(processHandle) function.
	/// </summary>
	/// <param name="processHandle"> A handle to the process. </param>
	/// <returns> The module base address of the process. </returns>
	CORVUS_API uintptr_t CORVUS_CALL GetModuleBaseAddressNt(const HANDLE processHandle);

	/// <summary>
	/// Initializes the PEB base address using processInfo instead of calling the GetPebBaseAddressNt() function.
	/// </summary>
	/// <param name="processHandle"> A handle to the process. </param>
	/// <param name="processInfo"> The extended native process structure containing the PEB base address. </param>
	/// <returns> The module base address of the process. </returns>
	CORVUS_API uintptr_t CORVUS_CALL GetModuleBaseAddressNt(
		const HANDLE processHandle,
		const PROCESS_EXTENDED_BASIC_INFORMATION& processInfo);

	/// <summary>
	/// Initializes the PEB using ReadVirtualMemoryNt<PEB>(processHandle, pebBaseAddress, peb)
	/// </summary>
	/// <param name="processHandle"> A handle to the process. </param>
	/// <param name="pebBaseAddress"> The PEB base address. </param>
	/// <returns></returns>
	CORVUS_API uintptr_t CORVUS_CALL GetModuleBaseAddressNt(
		const HANDLE processHandle,
		const uintptr_t pebBaseAddress);

	/// <summary>
	/// Directly uses the PEB from reference.
	/// </summary>
	/// <param name="processHandle"> A handle to the process. </param>
	/// <param name="pebBaseAddress"> The PEB base address. </param>
	/// <returns></returns>
	CORVUS_API uintptr_t CORVUS_CALL GetModuleBaseAddressNt(const HANDLE processHandle, const PEB& peb);

	/// <summary>
	/// IF ProcessWow64Information is not NULL, the process is running under WoW64 and is a 32-bit process.
	/// <para> If it is NULL, the process is running natively and is a 64-bit process. </para>
	/// </summary>
	/// <param name="processHandle"> A handle to the process. </param>
	/// <returns>
	/// Corvus::Object::ArchitectureType
	/// </returns>
	CORVUS_API Corvus::Object::ArchitectureType CORVUS_CALL GetArchitectureTypeNt(const HANDLE processHandle);

	CORVUS_API std::vector<LDR_DATA_TABLE_ENTRY> CORVUS_CALL GetProcessModulesNt(
		const HANDLE processHandle,
		const PEB& peb);

	/// <summary>
	/// Adds module entry objects to the list of module entry objects.
	/// <para> Does not do any kind of validation on the list. </para>
	/// </summary>
	/// <param name="processHandle"> A handle to the process. </param>
	/// <param name="processId"> The unique process identifier. </param>
	/// <param name="peb"> A const reference to the PEB. </param>
	/// <param name="modules"> A reference to the list of module entry objects. </param>
	/// <returns> TRUE if all values are sucessfully assigned. </returns>
	CORVUS_API BOOL CORVUS_CALL GetProcessModuleObjectsNt(
		const HANDLE processHandle,
		const DWORD processId,
		const PEB& peb,
		std::vector<Corvus::Object::ModuleEntry>& modules);

	CORVUS_API std::vector<SYSTEM_THREAD_INFORMATION> CORVUS_CALL
		GetProcessThreadsNt(
			const HANDLE processHandle,
			const DWORD processId);

	/// <summary>
	/// Adds thread entry objects to the list of thread entry objects.
	/// <para> Does not do any kind of validation on the list. </para>
	/// </summary>
	/// <param name="processHandle"> A handle to the process. </param>
	/// <param name="processId"> The unique process identifier. </param>
	/// <param name="threads"> A reference to the list of thread entry objects. </param>
	/// <returns> TRUE if all values are sucessfully assigned. </returns>
	CORVUS_API BOOL CORVUS_CALL GetProcessThreadObjectsNt(
		const HANDLE processHandle,
		const DWORD processId,
		std::vector<Corvus::Object::ThreadEntry>& threads);

	CORVUS_API std::vector<SYSTEM_HANDLE_TABLE_ENTRY_INFO> CORVUS_CALL GetProcessHandlesNt(
		const HANDLE processHandle,
		const DWORD processId);

	/// <summary>
	/// Adds handle entry objects to the list of handle entry objects.
	/// <para> Does not do any kind of validation on the list. </para>
	/// </summary>
	/// <param name="processHandle"> A handle to the process. </param>
	/// <param name="processId"> The unique process identifier. </param>
	/// <param name="handles"> A reference to the list of handle entry objects. </param>
	/// <returns> TRUE if all values are sucessfully assigned. </returns>
	CORVUS_API BOOL CORVUS_CALL GetProcessHandleObjectsNt(
		const HANDLE processHandle,
		const DWORD processId,
		std::vector<Corvus::Object::HandleEntry>& handles);

	CORVUS_API TOKEN_STATISTICS CORVUS_CALL GetProcessTokenStatisticsNt(const HANDLE tokenHandle);
	CORVUS_API std::vector<LUID_AND_ATTRIBUTES> CORVUS_CALL GetProcessTokenPriviligesNt(const HANDLE tokenHandle);

	CORVUS_API BOOL CORVUS_CALL GetProcessTokenPriviligeObjectsNt(
		const HANDLE tokenHandle,
		std::vector<Corvus::Object::PrivilegeEntry>& privileges);

	/// <summary>
	/// Requires SeTcbPrivilege.
	/// </summary>
	/// <param name="tokenHandle"> A handle to the token. </param>
	/// <returns> The session ID of the token. </returns>
	CORVUS_API DWORD CORVUS_CALL GetProcessTokenSessionIdNt(const HANDLE tokenHandle);
#pragma endregion

	/*
/// <summary>
/// EXPERIMENTAL: SYSTEM_EXTENDED_THREAD_INFORMATION @ SYSTEM_PROCESS_INFORMATION.
/// </summary>
/// <param name="processHandle"> A handle to the process. </param>
/// <param name="processId"> The unique process identifier. </param>
/// <returns> A list of SYSTEM_EXTENDED_THREAD_INFORMATION objects. </returns>
[[deprecated("Uses experimental NT structure: SYSTEM_EXTENDED_THREAD_INFORMATION @ SYSTEM_PROCESS_INFORMATION.")]]
std::vector<SYSTEM_EXTENDED_THREAD_INFORMATION> GetProcessThreadsExtendedNt(
	const HANDLE processHandle,
	const DWORD processId);
	*/

	/*
/// <summary>
/// Initializes the win32ThreadStartAddress and the tebBaseAddress.
/// <para> EXPERIMENTAL: SYSTEM_EXTENDED_THREAD_INFORMATION @ SYSTEM_PROCESS_INFORMATION. </para>
/// </summary>
/// <param name="processHandle"> A handle to the process. </param>
/// <param name="processId"> The unique process identifier. </param>
/// <returns> A list of ThreadEntry objects. </returns>
[[deprecated("Uses experimental NT structure: SYSTEM_EXTENDED_THREAD_INFORMATION @ SYSTEM_PROCESS_INFORMATION.")]]
BOOL GetProcessThreadObjectsExtendedNt(
	const HANDLE processHandle,
	const DWORD processId,
	std::vector<Corvus::Object::ThreadEntry>& threads);
	*/
}