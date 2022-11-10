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

		SetupImGui();
	}

	void EditorApp::OnTick(F32 dt) noexcept
	{
	}

	void EditorApp::OnRender() noexcept
	{
		
	}

	void EditorApp::OnPostRender() noexcept
	{
		OnGUI();
		Application::OnPostRender();
	}

	void EditorApp::OnGUI() noexcept
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Test Window");

		ImGui::End();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void EditorApp::OnShutdown() noexcept
	{
	}

	void EditorApp::SetupImGui()
	{
		auto d3d = Core::Globals::g_engineServices.ResolveService<Graphics::D3DManager>();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplWin32_Init(m_hWnd);
		ImGui_ImplDX11_Init(d3d->GetDevice(), d3d->GetContext());
		ImGui::StyleColorsDark();
	}
}