#include "editorpch.h"
#include "Editor/EditorApp.h"
#include "optick/include/optick.h"
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
	
	OPTICK_APP("ProximityEditor")

	return Proximity::Core::EngineMain(hInstance, new Proximity::Editor::EditorApp(hInstance));
}