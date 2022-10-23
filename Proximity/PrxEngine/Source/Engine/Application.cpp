#include "enginepch.h"
#include "Engine/Application.h"

namespace Proximity::Core
{
	Application::Application(HINSTANCE hInst)
	{
	}

	Application::~Application()
	{
	}

	bool Application::PreInit() noexcept
	{
		return false;
	}

	void Application::Run()
	{
		PreInit();
		OnStart();
		while (m_appWantsExit)
		{

		}
		OnShutdown();
		PostShutdown();
	}

	void Application::PostShutdown() noexcept
	{

	}
}