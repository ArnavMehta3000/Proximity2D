#include "enginepch.h"
#include "Engine/Application.h"

namespace Proximity::Core
{
	static const LPCWSTR s_className = TEXT("PROXIMITY");

	Application::Application(HINSTANCE hInst)
		:
		m_hInstance(hInst),
		m_appWantsExit(false),
		m_windowCreated(false),
		m_windowWidth(0),
		m_windowHeight(0),
		m_hWnd(NULL)
	{
		m_clientWidth  = GetSystemMetrics(SM_CXSCREEN);
		m_clientHeight = GetSystemMetrics(SM_CYSCREEN);
	}

	bool Application::PreInit() noexcept
	{
		PRX_LOG_DEBUG("Starting application Pre Initialization");

		bool result = true;
		result = m_windowCreated ? true: InitWindow();

		if (!Input::Init(m_hWnd))
			return false;

		// Register 2D renderer as an application level service and cache it
		m_renderer2D = g_engineServices.ResolveService<Graphics::Renderer2D>();
		if (!m_renderer2D)
		{
			PRX_LOG_FATAL("Failed to resolve Renderer2D as a service");
			return false;
		}

		PRX_LOG_DEBUG("Application Pre Initalization completed with result: %s", result ? "Success" : "Fail");
		return result;
	}
	static float t = 0.0f;  // TODO: REMOVE!!
	void Application::Run()
	{
		if (!PreInit())
		{
			PRX_LOG_FATAL("Failed spplication Pre Initialization");
			return;
		}

		OnStart();
		
		while (ProcessWindowMessages())
		{
			OnTick(0);

			OnPreRender();
			OnRender();
			OnPostRender();
		}

		PRX_LOG_DEBUG("Begin application shutdown procedure");
		OnShutdown();
		PostShutdown();
	}

	void Application::OnPreRender() noexcept
	{
		t += 0.01f;
		m_renderer2D->BeginRendering(Graphics::ClearCommand::Color(abs(sin(t)), 0.01f, abs(cos(t))));
		m_renderer2D->BeginFrame();
	}

	void Application::OnPostRender() noexcept
	{
		m_renderer2D->EndFrame();
	}

	void Application::PostShutdown() noexcept
	{
		PRX_LOG_DEBUG("Begin application post shutdown procedure");

		UnregisterClass(s_className, m_hInstance);
	}


	void Application::Test()
	{
		Math::I32 x = Input::GetMouseRelX();
		Math::I32 y = Input::GetMouseRelY();

		std::wstring wstr(std::to_wstring(x) + L" " + std::to_wstring(y));

		SetWindowText(m_hWnd, wstr.c_str());
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
			_T("Proximity Application"),
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

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		return 0;
	}
}