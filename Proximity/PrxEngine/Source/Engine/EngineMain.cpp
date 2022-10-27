#include "enginepch.h"
#include "Engine/EngineMain.h"
#include "Engine/Application.h"
#include <filesystem>

namespace Proximity::Core
{
	bool Init(const WindowDesc& windowDesc)
	{
		Utils::Logger::Init();
		Utils::Logger::SetFileOutput("EngineLog.prxlog");

		// TODO: Update directory paths, maybe set up directory at editor level?
		Utils::DirectoryManager::SetWorkingDirectory(std::filesystem::current_path());
		//Utils::DirectoryManager::CreateProject();
		
		
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

		// TODO: Debug that window has been created, update logger

		if (!Core::Init(app->GetWindowDesc()))
			return -1;

		// TODO: Engine initialized, change logger file output

		app->Run();
		Core::Shutdown();

		delete app;
		return 0;
	}
}