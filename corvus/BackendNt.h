#pragma once
#include "IWindowsBackend.h"

namespace Corvus::Data
{
	class BackendNt final : public IWindowsBackend
	{
	private:
		static PROCESS_EXTENDED_BASIC_INFORMATION QueryExtendedProcessInfo(HANDLE hProcess);
		static std::wstring QueryImageFilePathNt(HANDLE hProcess);
		static uintptr_t QueryModuleBaseAddress(DWORD processId, const std::wstring& processName);
		static Corvus::Object::UserProcessBasePriorityClass QueryPriorityClassNt(HANDLE hProcess);
		static Corvus::Object::ArchitectureType QueryArchitectureNt(HANDLE hProcess);
		static std::wstring QueryObjectNameNt(HANDLE hObject, DWORD processId);
		static std::wstring QueryObjectTypeNameNt(HANDLE hObject, DWORD processId);

	public:
		BackendNt() = default;
		~BackendNt() override = default;

		HANDLE OpenBackendHandle(const DWORD processId, const ACCESS_MASK accessMask) override;
		BOOL CloseBackendHandle(HANDLE handle) override;
		std::vector<Corvus::Object::ProcessEntry> QueryProcesses() override;
		Corvus::Object::ProcessEntry QueryProcessInfo(HANDLE hProcess, DWORD processId) override;
		std::vector<Corvus::Object::ModuleEntry> QueryModules(const Corvus::Object::ProcessObject& Object) override;
		std::vector<Corvus::Object::ThreadEntry> QueryThreads(const Corvus::Object::ProcessObject& Object) override;
		std::vector<Corvus::Object::HandleEntry> QueryHandles(const Corvus::Object::ProcessObject& Object) override;
	};
}