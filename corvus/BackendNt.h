#pragma once
#include "IProcessBackend.h"

namespace Corvus::Backend
{
	class BackendNt final : public IProcessBackend
	{
	private:
		static PROCESS_EXTENDED_BASIC_INFORMATION QueryExtendedProcessInfo(HANDLE hProcess);
		static std::wstring QueryImageFilePathNt(HANDLE hProcess);
		static uintptr_t QueryModuleBaseAddress(DWORD processId, const std::wstring& processName);
		static Corvus::Process::PriorityClass QueryPriorityClassNt(HANDLE hProcess);
		static Corvus::Process::ArchitectureType QueryArchitectureNt(HANDLE hProcess);
		static std::wstring QueryObjectNameNt(HANDLE hObject, DWORD processId);
		static std::wstring QueryObjectTypeNameNt(HANDLE hObject, DWORD processId);

	public:
		BackendNt() = default;
		~BackendNt() override = default;

		HANDLE OpenBackendHandle(const DWORD processId, const ACCESS_MASK accessMask) override;
		BOOL CloseBackendHandle(HANDLE handle) override;
		std::vector<Corvus::Process::ProcessEntry> QueryProcesses() override;
		Corvus::Process::ProcessEntry QueryProcessInfo(HANDLE hProcess, DWORD processId) override;
		std::vector<Corvus::Process::ModuleEntry> QueryModules(const Corvus::Process::WindowsProcess& Process) override;
		std::vector<Corvus::Process::ThreadEntry> QueryThreads(const Corvus::Process::WindowsProcess& Process) override;
		std::vector<Corvus::Process::HandleEntry> QueryHandles(const Corvus::Process::WindowsProcess& Process) override;
	};
}