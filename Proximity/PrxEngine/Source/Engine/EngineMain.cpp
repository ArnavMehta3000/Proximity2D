#include "enginepch.h"
#include "Engine/EngineMain.h"
#include "Engine/Application.h"

namespace Proximity::Core
{
	bool Init()
	{
		// Init logger
		// Init event broker
		// Init renderer
		// Init other systems...

		return true;
	}
	void Shutdown()
	{
	}
	int EngineMain(HINSTANCE hInstance, Proximity::Core::Application* app)
	{
		if (!Core::Init())
			return -1;

		app->Run();
		Core::Shutdown();

		delete app;
		return 0;
	}
}