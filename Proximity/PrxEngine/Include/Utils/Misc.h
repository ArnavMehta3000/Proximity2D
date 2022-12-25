#pragma once
#include <string>

namespace Proximity::Utils
{
	std::wstring ToWideString(const std::string& str);
	void ToWideString(const std::string& src, std::wstring& dest);
}