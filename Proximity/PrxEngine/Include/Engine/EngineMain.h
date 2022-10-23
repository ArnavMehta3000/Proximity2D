#pragma once


namespace Proximity::Core
{
	class Application;

	
	
	bool Init();      // Initialize engine level sub systems here
	void Shutdown();  // Shutdown engine level sub systems here

	int EngineMain(HINSTANCE hInstance, Proximity::Core::Application* app);	
}