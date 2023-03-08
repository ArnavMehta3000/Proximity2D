#pragma once

namespace Proximity::Core::Globals
{
#define PRX_RESOLVE(service) Core::Globals::g_engineServices.ResolveService<service>()

	// Engine level services
	inline Utils::ServiceLocator g_engineServices;

	// Action that gets fired when the window resizes
	inline Utils::Action<Math::U32, Math::U32> g_vpResizeEvent;

	inline bool g_engineIsSuspended = false;

	inline std::shared_ptr<Utils::TextBuffer> g_engineDebugBuffer = std::make_shared<Utils::TextBuffer>(1000);
	inline std::shared_ptr<Utils::TextBuffer> g_editorDebugBuffer = std::make_shared<Utils::TextBuffer>(1000);

}