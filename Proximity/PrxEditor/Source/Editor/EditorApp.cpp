#include "editorpch.h"
#include "Editor/EditorApp.h"

namespace Proximity::Editor
{
	EditorApp::EditorApp(HINSTANCE hInst) : Proximity::Core::Application(hInst)
	{
	}

	EditorApp::~EditorApp()
	{
	}

	void EditorApp::OnStart() noexcept
	{
		SetWindowText(this->m_hWnd, _T("Proximity2D Editor "));
	}

	void EditorApp::OnTick(F32 dt) noexcept
	{
	}

	void EditorApp::OnRender() noexcept
	{
	}

	void EditorApp::OnGUI() noexcept
	{
	}

	void EditorApp::OnShutdown() noexcept
	{
	}
}