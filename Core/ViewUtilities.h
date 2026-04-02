#pragma once
#include <WinUser.h>
#include <string>

namespace Muninn::View
{
	BOOL ShowMessageBox(
		const std::wstring& text,
		const std::wstring& title,
		UINT MB_Type = MB_OK) noexcept;
}