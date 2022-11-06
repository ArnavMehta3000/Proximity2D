#pragma once
#include <PrxRenderer2D.h>
using namespace Proximity::Math;

namespace Proximity::Core
{
	using WindowSize = std::tuple<F32, F32>;
	using ClientSize = std::tuple<F32, F32>;

	struct WindowDesc
	{
		WindowSize WindowSize;
		ClientSize ClientSize;
		HINSTANCE HInstance;
		HWND Handle;
	};

	class Application
	{
	public:
		Application(HINSTANCE hInst);
		virtual ~Application() {}

		void Run();

		HWND GetHandle()           const noexcept { return m_hWnd; }
		WindowSize GetWindowSize() const noexcept { return std::make_tuple((F32)m_windowWidth, (F32)m_windowHeight); }
		ClientSize GetClientSize() const noexcept { return std::make_tuple((F32)m_clientWidth, (F32)m_clientHeight); }
		WindowDesc GetWindowDesc() const noexcept { return { GetWindowSize(), GetClientSize(), m_hInstance, m_hWnd }; }

		bool InitWindow();

		static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	protected:
		virtual void OnStart() noexcept          = 0;  // Called after PreInit()
		virtual void OnTick(F32 dt) noexcept     = 0;  // Called once every frame
		virtual void OnRender() noexcept         = 0;  // Called once every frame after OnTick()
		virtual void OnShutdown() noexcept       = 0;  // Called when application wants to close

		virtual void OnPreRender() noexcept;
		virtual void OnPostRender() noexcept;

	private:		
		bool PreInit() noexcept;                       // Initialize all application level sub-systems
		void PostShutdown() noexcept;                  // Shutdown all application level sub-systems
		bool ProcessWindowMessages();

	protected:
		HINSTANCE               m_hInstance;
		bool                    m_appWantsExit;
		bool                    m_windowCreated;

		HWND m_hWnd;
		UINT m_windowWidth, m_windowHeight;
		UINT m_clientWidth, m_clientHeight;
	};
}