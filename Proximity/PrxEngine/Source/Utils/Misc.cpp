#include "enginepch.h"
#include "Utils/Misc.h"

namespace Proximity::Utils
{
	std::wstring ToWideString(const std::string& str)
	{
		std::wstring dest;
		dest.assign(str.begin(), str.end());
		return dest;
	}

	void ToWideString(const std::string& src, std::wstring& dest)
	{
		dest.assign(src.begin(), src.end());
	}
}