#pragma once
#include <iostream>
#include <ProcessController.h>

#ifndef BREAKLINE
#define BREAKLINE std::wcout << '\n'
#endif

void PrintTitle(const wchar_t* title) noexcept;
void PrintError(const wchar_t* error) noexcept;
void PrintConfig(const wchar_t* config) noexcept;
void PrintProcessId(Muninn::Controller::ProcessController* pProcessController) noexcept;
void PrintProcessHandle(Muninn::Controller::ProcessController* pProcessController) noexcept;
void PrintEntry(Muninn::Controller::ProcessController* pProcessController) noexcept;
void PrintModules(Muninn::Controller::ProcessController* pProcessController) noexcept;
void PrintSimpleInject( Muninn::Controller::ProcessController* pProcessController) noexcept;