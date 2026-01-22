#pragma once
#include <string>

namespace corvus::converter
{
    std::wstring StringToWString(const std::string& string);
    std::string WStringToString(const std::wstring& wstring);
}