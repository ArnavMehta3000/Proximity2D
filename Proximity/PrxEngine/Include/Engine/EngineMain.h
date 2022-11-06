#pragma once
#include "Engine/Application.h"
#include "Graphics/Rendering/Renderer2D.h"
#include <filesystem>

namespace Proximity::Core
{
	class Application;
	struct WindowDesc;

	// Private namespace to handle in engine services
	namespace
	{
		Utils::ServiceLocator* g_engineServices;
	}
	
	
	bool Init(const WindowDesc& windowDesc);      // Initialize engine level sub systems here
	void Shutdown();                              // Shutdown engine level sub systems here

	int EngineMain(HINSTANCE hInstance, Proximity::Core::Application* app);	
}