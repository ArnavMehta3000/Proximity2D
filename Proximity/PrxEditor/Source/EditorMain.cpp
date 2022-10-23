#include "pch.h"
#include <PrxEngine.h>
#include <Engine/EngineMain.h>  // Engine entry point

#include "Editor/EditorApp.h"

using namespace Proximity::Math;

// Editor entry point
int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nShowCmd);

	Proximity::Core::EngineMain(hInstance, new Proximity::Editor::EditorApp(hInstance));
}