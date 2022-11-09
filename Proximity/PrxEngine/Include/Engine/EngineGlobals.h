#pragma once

namespace Proximity::Core::Globals
{
	// Engine level services
	inline Utils::ServiceLocator g_engineServices;

	// Action that gets fired when the window resizes
	inline Utils::Action<Math::U32, Math::U32> g_resizeEvent;

	inline bool g_engineIsSuspended = false;
}