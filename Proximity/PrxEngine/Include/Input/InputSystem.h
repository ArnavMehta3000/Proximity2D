#pragma once
#include "Utils/Action.h"
#include "Input/Mouse.h"
#include "Input/Keyboard.h"

namespace Proximity::Core
{
	class Input
	{
		friend class Application;
	public:
		static Utils::Action<void> OnKeyboardEvent;
		static Utils::Action<void> OnMouseEvent;

	private:
		static bool Init();
		static void Poll();
		static void Shutdown();

		static DirectX::Keyboard*       m_keyboard;
		static DirectX::Mouse*          m_mouse;

		static DirectX::Keyboard::State m_kbState;
		static DirectX::Mouse::State    m_mouseState;
	};
}