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

	std::string Input::KeyCodeToString(KeyCode key) noexcept
	{
		switch (key)
		{
		using enum Proximity::Core::Input::KeyCode;

		case Tab:        return "Tab";
		case Backspace:  return "Backspace";
		case Clear:      return "Clear";
		case Return:     return "Return";
		case Shift:      return "Shift";
		case Control:    return "Control";
		case Alt:        return "Alt";
		case Pause:      return "Pause";
		case CapsLock:   return "CapsLock";
		case Escape:     return "Escape";
		case Spacebar:   return "Spacebar";
		case PageUp:     return "PageUp";
		case PageDown:   return "PageDown";
		case End:        return "End";
		case Home:       return "Home";
		case LeftArrow:  return "LeftArrow";
		case RightArrow: return "RightArrow";
		case UpArrow:    return "UpArrow";
		case DownArrow:  return "DownArrow";
		case Insert:     return "Insert";
		case Delete:     return "Delete";
		case Help:       return "Help";
		case Num0:       return "0";
		case Num1:       return "1";
		case Num2:       return "2";
		case Num3:       return "3";
		case Num4:       return "4";
		case Num5:       return "5";
		case Num6:       return "6";
		case Num7:       return "7";
		case Num8:       return "8";
		case Num9:       return "9";
		case A:          return "A";
		case B:          return "B";
		case C:          return "C";
		case D:          return "D";
		case E:          return "E";
		case F:          return "F";
		case G:          return "G";
		case H:          return "H";
		case I:          return "I";
		case J:          return "J";
		case K:          return "K";
		case L:          return "L";
		case M:          return "M";
		case N:          return "N";
		case O:          return "O";
		case P:          return "P";
		case Q:          return "Q";
		case R:          return "R";
		case S:          return "S";
		case T:          return "T";
		case U:          return "U";
		case V:          return "V";
		case W:          return "W";
		case X:          return "X";
		case Y:          return "Y";
		case Z:          return "Z";
		case Multiply:   return "*";
		case Add:        return "+";
		case Separator:  return "|";
		case Subtract:   return "-";
		case Decimal:    return ".";
		case Divide:     return "/";
		case F1:         return "F1";
		case F2:         return "F2";
		case F3:         return "F3";
		case F4:         return "F4";
		case F5:         return "F5";
		case F6:         return "F6";
		case F7:         return "F7";
		case F8:         return "F8";
		case F9:         return "F9";
		case F10:        return "F10";
		case F11:        return "F11";
		case F12:        return "F12";
		case LShift:     return "LShift";
		case RShft:      return "RShift";
		case LControl:   return "LControl";
		case RContorl:   return "RControl";
		default:         return "";
		}
	}

	Input::KeyCode Input::StringToKeyCode(const std::string key) noexcept
	{
		using enum Proximity::Core::Input::KeyCode;

		if ("Tab")        return Tab;
		if ("Backspace")  return Backspace;
		if ("Clear")      return Clear;
		if ("Return")     return Return;
		if ("Shift")      return Shift;
		if ("Control")    return Control;
		if ("Alt")        return Alt;
		if ("Pause")      return Pause;
		if ("CapsLock")   return CapsLock;
		if ("Escape")     return Escape;
		if ("Spacebar")   return Spacebar;
		if ("PageUp")     return PageUp;
		if ("PageDown")   return PageDown;
		if ("End")        return End;
		if ("Home")       return Home;
		if ("LeftArrow")  return LeftArrow;
		if ("RightArrow") return RightArrow;
		if ("UpArrow")    return UpArrow;
		if ("DownArrow")  return DownArrow;
		if ("Insert")     return Insert;
		if ("Delete")     return Delete;
		if ("Help")       return Help;
		if ("0")          return Num0;
		if ("1")          return Num1;
		if ("2")          return Num2;
		if ("3")          return Num3;
		if ("4")          return Num4;
		if ("5")          return Num5;
		if ("6")          return Num6;
		if ("7")          return Num7;
		if ("8")          return Num8;
		if ("9")          return Num9;
		if ("A")          return A;
		if ("B")          return B;
		if ("C")          return C;
		if ("D")          return D;
		if ("E")          return E;
		if ("F")          return F;
		if ("G")          return G;
		if ("H")          return H;
		if ("I")          return I;
		if ("J")          return J;
		if ("K")          return K;
		if ("L")          return L;
		if ("M")          return M;
		if ("N")          return N;
		if ("O")          return O;
		if ("P")          return P;
		if ("Q")          return Q;
		if ("R")          return R;
		if ("S")          return S;
		if ("T")          return T;
		if ("U")          return U;
		if ("V")          return V;
		if ("W")          return W;
		if ("X")          return X;
		if ("Y")          return Y;
		if ("Z")          return Z;
		if ("*")          return Multiply;
		if ("+")          return Add;
		if ("|")          return Separator;
		if ("-")          return Subtract;
		if (".")          return Decimal;
		if ("/")          return Divide;
		if ("F1")         return F1;
		if ("F2")         return F2;
		if ("F3")         return F3;
		if ("F4")         return F4;
		if ("F5")         return F5;
		if ("F6")         return F6;
		if ("F7")         return F7;
		if ("F8")         return F8;
		if ("F9")         return F9;
		if ("F10")        return F10;
		if ("F11")        return F11;
		if ("F12")        return F12;
		if ("LShift")     return LShift;
		if ("RShift")     return RShft;
		if ("LControl")   return LControl;
		if ("RControl")   return RContorl;

		return Num0;
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
			KeyInfo ki
			{
				.Key   = (Input::KeyCode)wParam,
				.State = KeyInfo::KeyState(false, true)
			};
			OnKeyDown(ki);
			break;
		}

		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			KeyInfo ki
			{
				.Key   = (Input::KeyCode)wParam,
				.State = KeyInfo::KeyState(true, false)
			};
			OnKeyUp(ki);
			break;
		}

		default:
			return;
		}
	}
}