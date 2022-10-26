#include "Utils/Misc.h"

namespace Proximity::Utils
{
#pragma warning(push)
// NOTE: Disabling conversion data loss warning
#pragma warning( disable : 4244 ) 

	std::string ToString(const std::wstring& wstr)
	{
		return std::string(wstr.begin(), wstr.end());
	}

	std::wstring ToStringW(const std::string& str)
	{
		return std::wstring(str.begin(), str.end());
	}

#pragma warning( pop )
}