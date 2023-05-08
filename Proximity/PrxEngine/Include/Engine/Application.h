#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "PrxRenderer2D.h"
#include "Input/InputSystem.h"
#include "Utils/DirectoryManager.h"
#include "Game/Scene.h"
#include "Audio/SoundSystem.h"

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

	struct AppTimeInfo
	{
		AppTimeInfo()
			:
			FrameTime(0.0f),
			UpdateTime(0.0f),
			RenderTime(0.0f)
		{}

		AppTimeInfo(F32 frame, F32 update, F32 render)
			:
			FrameTime(frame),
			UpdateTime(update),
			RenderTime(render)
		{}

		F32 FrameTime;
		F32 UpdateTime;
		F32 RenderTime;
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
		AppTimeInfo GetAppTimeInfo() noexcept     { return AppTimeInfo(m_frameTimer, m_updateTimer, m_renderTimer); }

		bool InitWindow();

		static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	protected:
		virtual void OnStart()      noexcept;  // Called after PreInit()
		virtual void OnTick(F32 dt) noexcept;  // Called once every frame
		virtual void OnRender()     noexcept;  // Called once every frame after OnTick()
		virtual void OnUI()         noexcept;  // UI rendering
		virtual void OnShutdown()   noexcept;  // Called when application wants to close

		virtual void OnPreRender()  noexcept;  // Clears the back buffers
		virtual void OnPostRender() noexcept;  // Post render of the world

		void CreateProjectDirectory(std::string projectName);
		std::optional<bool> OpenProjectDirectory(const std::filesystem::path& projectFolder);

	private:		
		bool PreInit() noexcept;       // Initialize all application level sub-systems
		void PostShutdown() noexcept;  // Shutdown all application level sub-systems
		bool ProcessWindowMessages();  
		void Present();                // Presents the back buffer

	protected:
		HINSTANCE             m_hInstance;
		bool                  m_appWantsExit;
		bool                  m_windowCreated;

		bool                  m_isWorkingDirectorySet;
		Utils::FilePath       m_workingDirectory;

		HWND                  m_hWnd;
		Math::U32             m_windowWidth, m_windowHeight;
		Math::U32             m_clientWidth, m_clientHeight;

		Graphics::Renderer2D* m_renderer2D;
		Core::SceneManager*   m_sceneManager;
		Audio::SoundSystem*   m_soundSystem;
	private:
		std::thread           m_audioThread;
		Utils::Timer          m_frameTimer;
		Utils::Timer          m_updateTimer;
		Utils::Timer          m_renderTimer;
	};
}