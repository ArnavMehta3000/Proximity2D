#include "enginepch.h"
#include "Engine/EngineMain.h"
#include "Engine/Application.h"
#include "Graphics/Renderer2D.h"
#include <filesystem>

namespace Proximity::Core
{
	bool Init(const WindowDesc& windowDesc)
	{
		const auto& [w, h] = windowDesc.ClientSize;
		if (!RENDERER2D->Init(windowDesc.Handle, static_cast<U32>(w), static_cast<U32>(h), true))
			return false;
		//Utils::DirectoryManager::CreateProject();

		// Init event broker
		// Init other systems...

		return true;
	}

	void Shutdown()
	{
		PRX_LOG_DEBUG("Begin engine shutdown");
		//Graphics::Renderer::Shutdown();
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