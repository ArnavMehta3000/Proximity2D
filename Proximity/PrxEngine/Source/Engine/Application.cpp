#include "enginepch.h"
#include "Engine/Application.h"
#include "Engine/Game/Scene.h"
#include "Engine/Modules/MaterialLibrary.h"
#include "Engine/Modules/ShaderLibrary.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Proximity::Core
{
	static const LPCWSTR s_className = L"PROXIMITY";

	Application::Application(HINSTANCE hInst)
		:
		m_hInstance(hInst),
		m_appWantsExit(false),
		m_windowCreated(false),
		m_windowWidth(0),
		m_windowHeight(0),
		m_hWnd(NULL),
		m_isWorkingDirectorySet(false),
		m_renderer2D(nullptr),
		m_frameTimer(Utils::Timer()),
		m_updateTimer(Utils::Timer()),
		m_renderTimer(Utils::Timer())
	{
		m_clientWidth  = GetSystemMetrics(SM_CXSCREEN);
		m_clientHeight = GetSystemMetrics(SM_CYSCREEN);
	}

	bool Application::PreInit() noexcept
	{
		PRX_LOG_DEBUG("Starting application Pre Initialization");
		bool result = true;

		// Resolve renderer from global engine servies and cache it
		m_renderer2D = PRX_RESOLVE(Graphics::Renderer2D);
		if (!m_renderer2D)
		{
			PRX_LOG_FATAL("Failed to resolve Renderer2D as an engine service");
			result = false;
		}

		// Initialize scene manager
		m_sceneManager   = Core::Globals::g_engineServices.ResolveOrRegisterService<Core::SceneManager>();
		auto shaderLib   = Core::Globals::g_engineServices.ResolveOrRegisterService<Modules::ShaderLibrary>();
		auto materialLib = Core::Globals::g_engineServices.ResolveOrRegisterService<Modules::MaterialLibrary>();

		Graphics::GPUShader::CreateDefaults();                                         // Create default VS/PS
		Graphics::Material mat(shaderLib->Get("Internal PS"), shaderLib->Get("Internal VS"), "INTERNAL_MAT_DEF_PS");  // Create material from default PS
		auto& x = mat.GetConstantBufferList();
		materialLib->AddMaterial(std::make_shared<Graphics::Material>(mat));

		PRX_LOG_DEBUG("Application Pre Initalization completed with result: %s", result ? "Success" : "Fail");
		return result;
	}

	void Application::Run()
	{
		if (!PreInit())
		{
			PRX_LOG_FATAL("Failed spplication Pre Initialization");
			return;
		}

		OnStart();

		m_frameTimer.Reset();
		m_frameTimer.Start();
		m_updateTimer.Reset();
		m_updateTimer.Start();
		m_renderTimer.Reset();
		m_renderTimer.Start();
		
		while (!m_appWantsExit && ProcessWindowMessages())
		{
			if (Globals::g_engineIsSuspended)
				continue;

			m_frameTimer.Tick();

			m_updateTimer.Reset();
			OnTick(m_frameTimer);
			m_updateTimer.Tick();

			m_renderTimer.Reset();
			OnPreRender();
			OnRender();
			OnPostRender();
			OnUI();
			Present();
			m_renderTimer.Tick();
		}

		PRX_LOG_DEBUG("Begin application shutdown procedure");
		OnShutdown();
		PostShutdown();
	}

	void Application::OnPreRender() noexcept
	{
		m_renderer2D->BindRenderTarget(Graphics::RenderTargetType::FRAME_BUFFER);
	}

	void Application::OnPostRender() noexcept
	{
		m_renderer2D->BindRenderTarget(Graphics::RenderTargetType::BACK_BUFFER);
	}

	void Application::Present()
	{
		m_renderer2D->EndFrame();
	}

	void Application::PostShutdown() noexcept
	{
		PRX_LOG_DEBUG("Begin application post shutdown procedure");

		UnregisterClass(s_className, m_hInstance);
	}

	void Application::CreateProjectDirectory()
	{
		using DM = Utils::DirectoryManager;
		
		DM::s_appDirectories.RootPath     = m_workingDirectory;
		DM::s_appDirectories.ScriptsPath  = m_workingDirectory / "Assets" / "Scripts";
		DM::s_appDirectories.ScenesPath   = m_workingDirectory / "Assets" / "Scenes";
		DM::s_appDirectories.TexturesPath = m_workingDirectory / "Assets" / "Textures";
		DM::s_appDirectories.ShadersPath  = m_workingDirectory / "Assets" / "Shaders";
		DM::s_appDirectories.AudioPath    = m_workingDirectory / "Assets" / "Audio";

		DM::CreateDir(DM::s_appDirectories.ScriptsPath );
		DM::CreateDir(DM::s_appDirectories.TexturesPath);
		DM::CreateDir(DM::s_appDirectories.ShadersPath );
		DM::CreateDir(DM::s_appDirectories.AudioPath   );
		
		Utils::Logger::SetFileOutput();
	}


	bool Proximity::Core::Application::InitWindow()
	{
		// Register class
		CREATE_ZERO(WNDCLASSEX, wcex);
		wcex.cbSize        = sizeof(WNDCLASSEX);
		wcex.style         = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc   = MessageRouter;
		wcex.cbClsExtra    = 0;
		wcex.cbWndExtra    = 0;
		wcex.hInstance     = m_hInstance;
		wcex.hIcon         = nullptr;
		wcex.hIconSm       = nullptr;
		wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName  = nullptr;
		wcex.lpszClassName = s_className;

		RegisterClassEx(&wcex);

		// Removing thick frame (responsible for resizing) and maximize box 
		DWORD style = WS_OVERLAPPEDWINDOW;
		RECT rc{ 0, 0, static_cast<LONG>(m_clientWidth), static_cast<LONG>(m_clientHeight) };
		AdjustWindowRect(&rc, style, FALSE);

		m_windowWidth = rc.right - rc.left;
		m_windowHeight = rc.bottom - rc.top;

		m_hWnd = CreateWindow(
			s_className,
			L"Proximity Application",
			style,
			CW_USEDEFAULT, CW_USEDEFAULT,
			m_windowWidth, m_windowHeight,
			nullptr,
			nullptr,
			m_hInstance,
			this
		);

		// TODO: Window creation eror handling
		if (!m_hWnd)
		{
			return false;
		}


		ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
		SetFocus(m_hWnd);
		m_windowCreated = true;

		PRX_LOG_DEBUG("Window successfully created");

		return true;
	}

	bool Application::ProcessWindowMessages()
	{
		MSG msg = { 0 };
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				// TODO: Maybe do input polling here...?
				return true;
			}
		}
		return false;
	}


	LRESULT Application::MessageRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		Application* window;

		if (msg == WM_CREATE)
		{
			window = (Application*)((LPCREATESTRUCT)lParam)->lpCreateParams;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);
		}
		else
		{
			window = (Application*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		}

		return window->MyWndProc(hWnd, msg, wParam, lParam);
	}

	
	LRESULT Application::MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		switch (msg)
		{
		case WM_DESTROY:
			m_appWantsExit = true;
			PostQuitMessage(0);
			break;

		case WM_ACTIVATE:
		case WM_INPUT:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
			Input::ProcessMouse(msg, wParam, lParam);
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			Input::ProcessKeyboard(msg, wParam, lParam);
			break;

		case WM_SIZE:
			m_clientWidth = LOWORD(lParam);
			m_clientHeight = HIWORD(lParam);

			RECT window; 
			GetWindowRect(hWnd, &window);
			m_windowWidth  = window.right - window.left;
			m_windowHeight = window.bottom - window.top;

			switch (wParam)
			{
			case SIZE_MAXIMIZED:
				Globals::g_engineIsSuspended = false;
				Core::Globals::g_vpResizeEvent(m_clientWidth, m_clientHeight);
				break;

			case SIZE_MINIMIZED:
				Globals::g_engineIsSuspended = true;
				break;

			case SIZE_RESTORED:
				Globals::g_engineIsSuspended = false;
			}
			break;

		case WM_EXITSIZEMOVE:
			if (!(m_clientWidth == 0 || m_clientHeight == 0))
				Core::Globals::g_vpResizeEvent(m_clientWidth, m_clientHeight);
			break;

		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 800;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 600;
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return 0;
	}
}