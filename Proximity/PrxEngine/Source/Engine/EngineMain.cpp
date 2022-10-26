#include "enginepch.h"
#include "Engine/EngineMain.h"
#include "Engine/Application.h"

namespace Proximity::Core
{
	bool Init(const WindowDesc& windowDesc)
	{
		Utils::Logger::Init();
		Utils::Logger::SetFileOutput("EngineLog.prxlog");

		PRX_LOG_DEBUG("This is a test %d %s", 5.5, "My name");
		PRX_LOG_INFO("This is a test %d %s", 5.5, "My name");
		PRX_LOG_WARN("This is a test %d %s", 5.5, "My name");
		PRX_LOG_ERROR("This is a test %d %s", 5.5, "My name");
		PRX_LOG_FATAL("This is a test %d %s", 5.5, "My name");
		// Init event broker
		// Init renderer
		// Init other systems...

		return true;
	}

	void Shutdown()
	{
		Utils::Logger::Shutdown();
	}

	int EngineMain(HINSTANCE hInstance, Proximity::Core::Application* app)
	{
		// Create window before initializing to get window desc
		app->InitWindow();

		if (!Core::Init(app->GetWindowDesc()))
			return -1;

		app->Run();
		Core::Shutdown();

		delete app;
		return 0;
	}
}