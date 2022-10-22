#pragma once
#include "Engine/EngineCore.h"

namespace Proximity
{
	namespace Core
	{
		int EngineMain(HINSTANCE hInstance, PWSTR pCmdLine, int nCmdShow)
		{
			EngineCore* engine = new EngineCore();
			
			if (!engine->Init(hInstance))
				return -1;

			engine->Run();
			engine->Shutdown();
			delete engine;
			return 0;
		}
	}
}