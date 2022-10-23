#pragma once

using namespace Proximity::Math;
namespace Proximity::Core
{
	class Application
	{
	public:
		Application(HINSTANCE hInst);
		virtual ~Application();

		void Run();


	protected:
		virtual void OnStart() noexcept          = 0;  // Called after PreInit()
		virtual void OnTick(F32 dt) noexcept     = 0;  // Called once every frame
		virtual void OnRender(F32 dt) noexcept   = 0;  // Called once every frame after OnTick()
		virtual void OnShutdown() noexcept       = 0;  // Called when application wants to close

	private:
		// Initialize all application level sub-systems
		bool PreInit() noexcept;  
		// Shutdown all application level sub-systems
		void PostShutdown() noexcept;


	protected:
		HINSTANCE m_hInstance;
		bool m_appWantsExit;
	};
}