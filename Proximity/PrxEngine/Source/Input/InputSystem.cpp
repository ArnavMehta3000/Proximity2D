#include "enginepch.h"
#include "Input/InputSystem.h"
#include <hidusage.h>  // For HID Defines

namespace Proximity::Core
{
	Utils::Action<void>       Input::OnMouseLBDown;
	Utils::Action<void>       Input::OnMouseRBDown;
	Utils::Action<void>       Input::OnMouseMBDown;
	Utils::Action<void>       Input::OnMouseLBUp;
	Utils::Action<void>       Input::OnMouseRBUp;
	Utils::Action<void>       Input::OnMouseMBUp;
	Utils::Action<Math::U16>  Input::OnMouseScroll;

#ifdef _DEBUG
	Utils::Action<void>       Input::OnMouseMove;
#endif // _DEBUG


	Utils::Action<Input::KeyInfo> Input::OnKeyDown;
	Utils::Action<Input::KeyInfo> Input::OnKeyUp;

	Math::I32 Input::s_mouseX;
	Math::I32 Input::s_mouseY;
	Math::I32 Input::s_mouseRelX;
	Math::I32 Input::s_mouseRelY;






	bool Input::Init(HWND window)
	{
		RAWINPUTDEVICE rid[1];
		rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		rid[0].usUsage     = HID_USAGE_GENERIC_MOUSE;
		rid[0].dwFlags     = RIDEV_INPUTSINK;
		rid[0].hwndTarget  = window;

		if (RegisterRawInputDevices(rid, 1, sizeof(rid[0])) == FALSE)
		{
			PRX_LOG_FATAL("Failed to initialize raw input device: Mouse");
			return false;
		}

		PRX_LOG_DEBUG("Input sub-system initialized: Success");
		return true;
	}

	void Input::ProcessMouse(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			// Handle raw input
		case WM_INPUT:
		{
			UINT dwSize = sizeof(RAWINPUT);
			static BYTE lpb[sizeof(RAWINPUT)];

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				s_mouseRelX = raw->data.mouse.lLastX;
				s_mouseRelY = raw->data.mouse.lLastY;
			}
		}
			break;

		case  WM_MOUSEMOVE:
			POINTS pt = MAKEPOINTS(lParam);
			s_mouseX = pt.x;
			s_mouseY = pt.y;
#ifdef _DEBUG
			OnMouseMove();
#endif // _DEBUG


		case WM_LBUTTONUP:
			OnMouseLBUp();
			break;

		case WM_LBUTTONDOWN:
			OnMouseLBDown();
			break;

		case WM_RBUTTONUP:
			OnMouseRBUp();
			break;

		case WM_RBUTTONDOWN:
			OnMouseRBDown();
			break;

		case WM_MBUTTONUP:
			OnMouseMBUp();
			break;

		case WM_MBUTTONDOWN:
			OnMouseMBDown();
			break;

		case WM_MOUSEWHEEL:
			OnMouseScroll(GET_WHEEL_DELTA_WPARAM(wParam));
			break;

		default:
			return;
		}
	}

	void Input::ProcessKeyboard(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			KeyInfo ki{ (Input::KeyCode)wParam };
			OnKeyDown(ki);
			break;
		}

		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			KeyInfo ki{ (Input::KeyCode)wParam };
			OnKeyUp(ki);
			break;
		}

		default:
			return;
		}
	}
}