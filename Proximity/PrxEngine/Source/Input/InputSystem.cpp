#include "enginepch.h"
#include "Input/InputSystem.h"

namespace Proximity::Core
{
	Utils::Action<void>       Input::OnMouseLBDown;
	Utils::Action<void>       Input::OnMouseRBDown;
	Utils::Action<void>       Input::OnMouseMBDown;
	Utils::Action<void>       Input::OnMouseLBUp;
	Utils::Action<void>       Input::OnMouseRBUp;
	Utils::Action<void>       Input::OnMouseMBUp;
	Utils::Action<Math::U16>  Input::OnMouseScroll;

	Utils::Action<Input::KeyInfo> Input::OnKeyDown;
	Utils::Action<Input::KeyInfo> Input::OnKeyUp;

	Math::I32 Input::s_mouseX;
	Math::I32 Input::s_mouseY;

	void Input::ProcessMouse(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case  WM_MOUSEMOVE:
			POINTS pt = MAKEPOINTS(lParam);
			s_mouseX = pt.x;
			s_mouseY = pt.y;

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