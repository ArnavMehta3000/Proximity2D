#include "enginepch.h"
#include "Engine/EngineMain.h"
#include "Engine/Application.h"
#include <filesystem>

namespace Proximity::Core
{
	bool Init(const WindowDesc& windowDesc)
	{
		
		
		//Utils::DirectoryManager::CreateProject();
		
		// Init event broker
		// Init renderer
		// Init other systems...

		return true;
	}

	void Shutdown()
	{
		PRX_LOG_DEBUG("Begin engine shutdown");
		Utils::Logger::Shutdown();
	}

	int EngineMain(HINSTANCE hInstance, Proximity::Core::Application* app)
	{
		// TODO: Update directory paths, maybe set up directory at editor level?
		Utils::DirectoryManager::SetWorkingDirectory(std::filesystem::current_path());
		PRX_LOG_DEBUG("Set working directory: %s", Utils::DirectoryManager::GetWorkingDir());

		Utils::Logger::Init();

		// Create window before initializing to get window desc
		if (!app->InitWindow())
		{
			PRX_LOG_FATAL("Failed to create window. Exiting...");
			return -1;
		}

		// TODO: Debug that window has been created, update logger

		if (!Core::Init(app->GetWindowDesc()))
		{
			PRX_LOG_FATAL("Failed to initialize engine core. Exiting...");
			return -1;
		}

		app->Run();
		Core::Shutdown();

		delete app;
		return 0;
	}
}