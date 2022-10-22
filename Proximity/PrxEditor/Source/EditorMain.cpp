#include "pch.h"
#include <PrxEngine.h>
#include "EngineMain.h"

using namespace Proximity::Math;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	U32 code = Proximity::Core::EngineMain(hInstance, lpCmdLine, nShowCmd);
	return code;
}