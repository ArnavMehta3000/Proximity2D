#pragma once


namespace Proximity::Core
{
	class Application;
	struct WindowDesc;
	
	
	bool Init(const WindowDesc& windowDesc);      // Initialize engine level sub systems here
	void Shutdown();                              // Shutdown engine level sub systems here

	int EngineMain(HINSTANCE hInstance, Proximity::Core::Application* app);	
}