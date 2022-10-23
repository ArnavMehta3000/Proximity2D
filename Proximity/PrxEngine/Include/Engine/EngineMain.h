#pragma once
#include "Application.h"


namespace Proximity::Core
{
	// Initialize engine level sub systems here
	bool Init();
	// Shutdown engine level sub systems here
	void Shutdown();

	int EngineMain(HINSTANCE hInstance, Proximity::Core::Application* app)
	{
		Core::Init();
		app->Run();
		Core::Shutdown();

		delete app;
		return 0;
	}
}