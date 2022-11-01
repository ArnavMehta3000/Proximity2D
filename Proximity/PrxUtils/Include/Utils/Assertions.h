#pragma once

namespace Proximity::Utils
{
	void ReportAssertionFailure(const char* expression, const char* message);
}
#define MSG_BOX(msg, caption)\
{\
	MessageBox(NULL, TEXT(msg), TEXT(caption), MB_OK | MB_ICONERROR);\
}

#define PRX_ASSERT(expr)                                    \
{                                                           \
	if (expr)                                               \
	{ }                                                     \
	else                                                    \
	{                                                       \
		Proximity::Utils::ReportAssertionFailure(#expr, "");\
		MSG_BOX("Assertion failed", "Error");               \
	}                                                       \
}

#define PRX_ASSERT_HR(hr, msg)                             \
{                                                          \
	if (FAILED(hr))                                        \
	{                                                      \
		Proximity::Utils::ReportAssertionFailure(#hr, msg);\
		MSG_BOX(msg, #hr);                     \
	}                                                      \
}