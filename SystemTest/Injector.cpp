#include "Injector.h"
#include "Log.h"

void SimpleInjectDll(Muninn::Controller::ProcessController* pProcessController) noexcept
{
	if (pProcessController->GetInjector().IsInjected == true)
	{
		Log(ConsoleColor::Red, "You've already injected.");
		return;
	}

	bool isDllPathASet{
		pProcessController->GetInjector().DllPathA != nullptr };

	bool isDllPathWSet{
		pProcessController->GetInjector().DllPathW != nullptr };

	if (!isDllPathASet && !isDllPathWSet)
	{
		Log(ConsoleColor::Red, "Please set the injector DLL path first.");
		return;
	}

	if (isDllPathASet)
		pProcessController->SimpleDLLInjectA();

	if (isDllPathWSet)
		pProcessController->SimpleDLLInjectW();

	if (!pProcessController->GetInjector().IsInjected)
		Log(ConsoleColor::Red, "Failed to inject DLL using the path.");

	if (pProcessController->GetInjector().IsInjected)
		Log(ConsoleColor::Yellow, "DLL injected successfully using the path.");

	return;
}