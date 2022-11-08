#pragma once
#include "Utils/Action.h"

// Raw input reference
// https://learn.microsoft.com/en-us/windows/win32/dxtecharts/taking-advantage-of-high-dpi-mouse-movement?redirectedfrom=MSDN

namespace Proximity::Core
{
	class Input
	{
		friend class Application;
	public:
		// NOTE: Not all keycodes implemented
		// Ref:  https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
		enum class KeyCode : unsigned char
		{
			// Special Keys
			Tab        = VK_TAB,
			Backspace  = VK_BACK,
			Clear      = VK_CLEAR,
			Return     = VK_RETURN,
			Shift      = VK_SHIFT,
			Control    = VK_CONTROL,
			Alt        = VK_MENU,
			Pause      = VK_PAUSE,
			CapsLock   = VK_CAPITAL,
			Escape     = VK_ESCAPE,
			Spacebar   = VK_SPACE,
			PageUp     = VK_PRIOR,
			PageDown   = VK_NEXT,
			End        = VK_END,
			Home       = VK_HOME,
			LeftArrow  = VK_LEFT,
			RightArroe = VK_RIGHT,
			UpArrow    = VK_UP,
			DownArrow  = VK_DOWN,
			Insert     = VK_INSERT,
			Delete     = VK_DELETE,
			Help       = VK_HELP,

			// Numbers
			Num0 = 0x30,
			Num1 = 0x31,
			Num2 = 0x32,
			Num3 = 0x33,
			Num4 = 0x34,
			Num5 = 0x35,
			Num6 = 0x36,
			Num7 = 0x37,
			Num8 = 0x38,
			Num9 = 0x39,

			// Alphabets
			A = 0x41,
			B = 0x42,
			C = 0x43,
			D = 0x44,
			E = 0x45,
			F = 0x46,
			G = 0x47,
			H = 0x48,
			I = 0x49,
			J = 0x4a,
			K = 0x4b,
			L = 0x4c,
			M = 0x4d,
			N = 0x4e,
			O = 0x4f,
			P = 0x50,
			Q = 0x51,
			R = 0x52,
			S = 0x53,
			T = 0x54,
			U = 0x55,
			V = 0x56,
			W = 0x57,
			X = 0x58,
			Y = 0x59,
			Z = 0x5a,

			// Operators
			Multiply  = VK_MULTIPLY,
			Add       = VK_ADD,
			Separator = VK_SEPARATOR,  // Pipe key ( | )
			Subtract  = VK_SUBTRACT,
			Decimal   = VK_DECIMAL,
			Divide    = VK_DIVIDE,

			// Function
			F1  = VK_F1,
            F2  = VK_F2,
            F3  = VK_F3,
            F4  = VK_F4,
            F5  = VK_F5,
            F6  = VK_F6,
            F7  = VK_F7,
            F8  = VK_F8,
            F9  = VK_F9,
            F10 = VK_F10,
            F11 = VK_F11,
            F12 = VK_F12,

			// Specifics
			LShift   = VK_LSHIFT,
			RShft    = VK_LSHIFT,
			LCOntrol = VK_LCONTROL,
			RContorl = VK_RCONTROL,
		};

		struct KeyInfo
		{
			KeyCode Key;
		};

	public:
		static bool Init(HWND window);

	public:
		static Utils::Action<void>       OnMouseLBDown;
		static Utils::Action<void>       OnMouseRBDown;
		static Utils::Action<void>       OnMouseMBDown;
		static Utils::Action<void>       OnMouseLBUp;
		static Utils::Action<void>       OnMouseRBUp;
		static Utils::Action<void>       OnMouseMBUp;

#ifdef _DEBUG
		static Utils::Action<void>       OnMouseMove;
#endif // _DEBUG

		
		static Utils::Action<Math::U16>  OnMouseScroll;

		static Utils::Action<KeyInfo> OnKeyDown;
		static Utils::Action<KeyInfo> OnKeyUp;

		static inline Math::I32 GetMouseX() noexcept { return s_mouseX; };
		static inline Math::I32 GetMouseY() noexcept { return s_mouseY; };

		static inline Math::I32 GetMouseRelX() noexcept { return s_mouseRelX; };
		static inline Math::I32 GetMouseRelY() noexcept { return s_mouseRelY; };

	private:
		static void ProcessMouse(UINT msg, WPARAM wParam, LPARAM lParam);
		static void ProcessKeyboard(UINT msg, WPARAM wParam, LPARAM lParam);

		static Math::I32 s_mouseX;
		static Math::I32 s_mouseY;

		static Math::I32 s_mouseRelX;
		static Math::I32 s_mouseRelY;
	};
}