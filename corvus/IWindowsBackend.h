#pragma once
#include "ProcessStructures.h"
#include <vector>

namespace Corvus::Object
{
	class ProcessObject;
}

namespace Corvus::Data
{
	class IWindowsBackend
	{
	public:
		IWindowsBackend() = default;
		virtual ~IWindowsBackend() = default;

		virtual HANDLE OpenBackendHandle(const DWORD processId, const ACCESS_MASK accessMask) = 0;
		virtual BOOL CloseBackendHandle(HANDLE handle) = 0;
		virtual std::vector<Corvus::Object::ProcessEntry> QueryProcesses() = 0;
		virtual Corvus::Object::ProcessEntry QueryProcessInfo(HANDLE hProcess, DWORD processId) = 0;
		virtual std::vector<Corvus::Object::ModuleEntry> QueryModules(const Corvus::Object::ProcessObject& Object) = 0;
		virtual std::vector<Corvus::Object::ThreadEntry> QueryThreads(const Corvus::Object::ProcessObject& Object) = 0;
		virtual std::vector<Corvus::Object::HandleEntry> QueryHandles(const Corvus::Object::ProcessObject& Object) = 0;
	};
}