#pragma once
#include <string>

namespace Proximity::Utils
{
	std::string ToString(const std::wstring& wstr);
	std::wstring ToStringW(const std::string& str);
}