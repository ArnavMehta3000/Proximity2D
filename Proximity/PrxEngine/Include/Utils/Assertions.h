#pragma once
#include <tchar.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Proximity::Utils
{
	void ReportAssertionFailure(const char* expression, const char* message);
}
#define MSG_BOX(msg, caption)                                     \
{                                                                 \
	MessageBoxA(NULL, (msg), (caption), MB_OK | MB_ICONERROR); \
}
																  
#define PRX_ASSERT(expr)                                          \
{                                                                 \
	if (expr)                                                     \
	{                                                             \
		Proximity::Utils::ReportAssertionFailure(#expr, "");      \
		MSG_BOX("Assertion failed", "Error");                     \
	}                                                             \
}																  
																  
#define PRX_ASSERT_MSG(expr, msg)                                 \
{                                                                 \
	if (expr)                                                     \
	{                                                             \
		Proximity::Utils::ReportAssertionFailure(#expr, "");      \
		MSG_BOX(msg, "Error");                                    \
	}                                                             \
}																  
																  
#define PRX_ASSERT_HR(hr, msg)                                    \
{                                                                 \
	if (FAILED(hr))                                               \
	{                                                             \
		Proximity::Utils::ReportAssertionFailure(#hr, msg);       \
		MSG_BOX(msg, #hr);                                        \
	}                                                             \
}																  
																  
#define PRX_FAIL_HR(expr)                                         \
{                                                                 \
	if (FAILED(expr))                                             \
		return false;                                             \
}																  