#include "ViewUtilities.h"

namespace Muninn::View
{
	BOOL ShowMessageBox(
		const std::wstring& text,
		const std::wstring& title,
		UINT MB_Type) noexcept
	{
		// WinUser.h
		return ::MessageBoxW(
			nullptr,
			text.c_str(),
			title.c_str(),
			MB_Type);
	}
}