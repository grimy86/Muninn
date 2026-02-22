#pragma once
#include "IWindowsBackend.h"

namespace Corvus::Data
{
	class Backend32 final : public IWindowsBackend
	{
	private:
		std::wstring QueryImageFilePath(HANDLE hProcess);
		static uintptr_t QueryModuleBaseAddress(DWORD processId, const std::wstring& processName);
		static Corvus::Object::UserProcessBasePriorityClass QueryPriorityClass(HANDLE hProcess);
		static bool QueryVisibleWindow(DWORD processId);
		static Corvus::Object::ArchitectureType QueryArchitecture(HANDLE hProcess, BOOL& isWow64);

	public:
		Backend32() = default;
		~Backend32() override = default;

		HANDLE OpenBackendHandle(const DWORD processId, const ACCESS_MASK accessMask) override;
		BOOL CloseBackendHandle(HANDLE handle) override;
		std::vector<Corvus::Object::ProcessEntry> QueryProcesses() override;
		Corvus::Object::ProcessEntry QueryProcessInfo(HANDLE hProcess, DWORD processId) override;
		std::vector<Corvus::Object::ModuleEntry> QueryModules(const Corvus::Object::ProcessObject& Object) override;
		std::vector<Corvus::Object::ThreadEntry> QueryThreads(const Corvus::Object::ProcessObject& Object) override;
		std::vector<Corvus::Object::HandleEntry> QueryHandles(const Corvus::Object::ProcessObject& Object) override;

		static bool QuerySeDebugPrivilege32(HANDLE hProcess);
		static int QueryThreadPriority32(HANDLE hThread);
	};
}