#include "enginepch.h"
#include "Engine/EngineMain.h"
#include "Graphics/Rendering/Renderer2D.h"
#include "Engine/Modules/ShaderLibrary.h"
#include "Engine/Modules/MaterialLibrary.h"
#include "Engine/Modules/TextureLibrary.h"
#include "Engine/Modules/AudioLibrary.h"
#include "Engine/Modules/ScriptLibrary.h"
#include "optick/include/optick.h"


namespace Proximity::Core
{
	int EngineMain(HINSTANCE hInstance, Proximity::Core::Application* app)
	{
		OPTICK_EVENT("Core::EngineMain")
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
		OPTICK_EVENT("Core::Init")
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


		// Register audio engine
		Globals::g_engineServices.RegisterService<Audio::SoundSystem>();
		PRX_LOG_DEBUG("Finished registering Sound System");


		// Register asset libraries
		Globals::g_engineServices.RegisterService<Modules::ShaderLibrary>();
		Globals::g_engineServices.RegisterService<Modules::MaterialLibrary>();
		Globals::g_engineServices.RegisterService<Modules::AudioLibrary>();
		Globals::g_engineServices.RegisterService<Modules::ScriptLibrary>();
		Globals::g_engineServices.ResolveOrRegisterService<Modules::TextureLibrary>()->Init();

		PRX_LOG_DEBUG("Finished registering asset libraries");

		return true;
	}

	void Shutdown()
	{
		OPTICK_EVENT("Core::Shutdown")
		PRX_LOG_INFO("Begin engine shutdown");

		PRX_RESOLVE(Graphics::Renderer2D)->Shutdown();
		PRX_RESOLVE(Modules::ShaderLibrary)->Release();
		PRX_RESOLVE(Modules::MaterialLibrary)->Release();
		PRX_RESOLVE(Modules::TextureLibrary)->Release();

		Utils::Logger::Shutdown();
	}
}