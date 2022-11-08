#include "enginepch.h"
#include "Input/InputSystem.h"

namespace Proximity::Core
{
	bool Input::Init()
	{
		PRX_LOG_DEBUG("Initialized input subsystem");
		return true;
	}

	void Input::Poll()
	{
		// Process input here
		m_kbState = m_keyboard->GetState();
		m_mouseState = m_mouse->GetState();
	}

	void Input::Shutdown()
	{
		delete m_keyboard;
		delete m_mouse;
	}
}