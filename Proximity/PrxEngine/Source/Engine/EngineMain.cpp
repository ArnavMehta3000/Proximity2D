#include "enginepch.h"
#include "Engine/EngineMain.h"


namespace Proximity::Core
{
	int EngineMain(HINSTANCE hInstance, Proximity::Core::Application* app)
	{
		// TODO: Update directory paths, maybe set up directory at editor level?
		Utils::DirectoryManager::SetWorkingDirectory(std::filesystem::current_path());
		PRX_LOG_DEBUG("Set working directory: %s", Utils::DirectoryManager::GetWorkingDir());

		// Init logger before anything else - important!
		Utils::Logger::Init();

		// Create window before initializing to get window desc
		if (!app->InitWindow())
		{
			PRX_LOG_FATAL("Failed to create window. Exiting...");
			return -1;
		}

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



	bool Init(const WindowDesc& windowDesc)
	{
		if (!Input::Init(windowDesc.Handle))
			return false;

		// Register renderer2D
		auto& [width, height] = windowDesc.ClientSize;
		auto renderer = g_engineServices.ResolveOrRegisterService<Graphics::Renderer2D>();
		renderer->Init(windowDesc.Handle, static_cast<Math::U32>(width), static_cast<Math::U32>(height), true);

		return true;
	}

	void Shutdown()
	{
		PRX_LOG_DEBUG("Begin engine shutdown");
		Utils::Logger::Shutdown();
	}
}