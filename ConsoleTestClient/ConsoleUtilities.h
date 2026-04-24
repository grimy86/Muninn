#pragma once
#include <iostream>
#include <ProcessController.h>

#ifndef BREAKLINE
#define BREAKLINE std::wcout << '\n'
#endif

enum class ConsoleColor : WORD
{
	Black = 0ul,
	Blue = FOREGROUND_BLUE,
	Green = FOREGROUND_GREEN,
	Cyan = FOREGROUND_GREEN | FOREGROUND_BLUE,
	Red = FOREGROUND_RED,
	Magenta = FOREGROUND_RED | FOREGROUND_BLUE,
	Yellow = FOREGROUND_RED | FOREGROUND_GREEN,
	White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	Gray = FOREGROUND_INTENSITY,
	BrightBlue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	BrightGreen = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	BrightCyan = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	BrightRed = FOREGROUND_RED | FOREGROUND_INTENSITY,
	BrightMagenta = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	BrightYellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	BrightWhite = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
};

template<typename... Args>
void Log(ConsoleColor color, Args&&... args) noexcept
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, static_cast<WORD>(color));

	(std::cout << ... << args) << '\n';

	SetConsoleTextAttribute(h, static_cast<WORD>(ConsoleColor::White));
}

template<typename... Args>
void LogW(ConsoleColor color, Args&&... args) noexcept
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, static_cast<WORD>(color));

	(std::wcout << ... << args) << L'\n';

	SetConsoleTextAttribute(h, static_cast<WORD>(ConsoleColor::White));
}

void PrintProcessHandle(Muninn::Controller::ProcessController* pProcessController) noexcept;
void PrintEntry(Muninn::Controller::ProcessController* pProcessController) noexcept;
void PrintModules(Muninn::Controller::ProcessController* pProcessController) noexcept;
void PrintSimpleInject( Muninn::Controller::ProcessController* pProcessController) noexcept;