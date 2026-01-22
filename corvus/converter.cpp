#include "converter.hpp"

namespace corvus::converter
{
	std::wstring StringToWString(const std::string& string)
	{
		return std::wstring{ string.begin(), string.end() };
	}

	std::string WStringToString(const std::wstring& wstring)
	{
		return std::string{ wstring.begin(), wstring.end() };
	}
}