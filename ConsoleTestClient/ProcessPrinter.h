#include <iostream>
#include <ProcessController.h>

void PrintTitle(HANDLE hConsole, const wchar_t* title) noexcept;
void PrintError(HANDLE hConsole, const wchar_t* error) noexcept;
void PrintConfig(HANDLE hConsole, const wchar_t* config) noexcept;
void PrintProcessId(Muninn::Controller::ProcessController* pProcessController) noexcept;
void PrintProcessHandle(Muninn::Controller::ProcessController* pProcessController) noexcept;
void PrintEntry(Muninn::Controller::ProcessController* pProcessController) noexcept;
void PrintModules(Muninn::Controller::ProcessController* pProcessController) noexcept;