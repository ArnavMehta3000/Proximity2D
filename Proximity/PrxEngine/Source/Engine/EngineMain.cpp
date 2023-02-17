#include "enginepch.h"
#include "Graphics/Rendering/Renderer2D.h"
#include "Engine/Modules/ShaderLibrary.h"
#include "Engine/EngineMain.h"


namespace Proximity::Core
{
	int EngineMain(HINSTANCE hInstance, Proximity::Core::Application* app)
	{
		Globals::g_debugBuffer = std::make_unique<Proximity::Utils::TextBuffer>(5);

		// Init logger before anything else - important!
		Utils::Logger::Init();
		// Create window before initializing to get window desc
		if (!app->InitWindow())
		{
			PRX_LOG_FATAL("Failed to create window. Exiting...");
			return -1;
		}
		Utils::DirectoryManager::Init(app->GetHandle());

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
		auto renderer = Globals::g_engineServices.ResolveOrRegisterService<Graphics::Renderer2D>();
		if (!renderer->Init(windowDesc.Handle, static_cast<Math::U32>(width), static_cast<Math::U32>(height), true))
		{
			PRX_LOG_FATAL("Failed to initialize Renderer2D");
			return false;
		}

		Globals::g_engineServices.RegisterService<Modules::ShaderLibrary>();


		return true;
	}

	void Shutdown()
	{
		PRX_LOG_DEBUG("Begin engine shutdown");

		Globals::g_engineServices.ResolveService<Graphics::Renderer2D>()->Shutdown();
		Globals::g_engineServices.ResolveService<Modules::ShaderLibrary>()->Release();

		Utils::Logger::Shutdown();
	}
}