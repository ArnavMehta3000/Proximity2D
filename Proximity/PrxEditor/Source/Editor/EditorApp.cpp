#include "editorpch.h"
#include "Editor/EditorApp.h"
#include "Editor/Panels/ScenePanel.h"
#include "Editor/Panels/ViewportPanel2D.h"
#include "Editor/Panels/BrowserPanel.h"
#include "Editor/Panels/DetailsPanel.h"
#include "Editor/Panels/EditorConsolePanel.h"
#include "Editor/Panels/AssetInfoPanel.h"
#include "Editor/Panels/ScriptEditorPanel.h"
#include "optick/include/optick.h"

namespace Proximity::Editor
{
	EditorApp::EditorApp(HINSTANCE hInst)
		: 
		Proximity::Core::Application(hInst),
		m_showAppStatsWindow(false),
		m_editorCam(),
		m_sceneState(SceneState::Edit)
	{}

	void EditorApp::OnStart() noexcept
	{
		OPTICK_EVENT("EditorApp::OnStart")

		Application::OnStart();

		SetWindowText(this->m_hWnd, _T("Proximity2D Editor "));
		SetupImGui();
		m_editorCam.OrthoScale(10.0f);
		m_editorCam.Position(Vec3(0, 0, -100));
		
		
		// Create panels
		m_editorPanels.push_back(new Panels::ScenePanel(&m_editorCam));       // 0
		m_editorPanels.push_back(new Panels::ViewportPanel2D(&m_editorCam));  // 1
		m_editorPanels.push_back(new Panels::BrowserPanel());                 // 2
		m_editorPanels.push_back(new Panels::DetailsPanel());                 // 3
		m_editorPanels.push_back(new Panels::EditorConsolePanel());           // 4
		m_editorPanels.push_back(new Panels::AssetInfoPanel());               // 5
		m_editorPanels.push_back(new Panels::ScriptEditorPanel());            // 6
	}

	void EditorApp::OnTick(F32 dt) noexcept
	{
		OPTICK_EVENT("EditorApp::OnTick")

		if (Core::Globals::g_engineIsSuspended)
			return;

		Application::OnTick(dt);

		auto scene = m_sceneManager->GetActiveScene();
		if (scene && m_sceneState == SceneState::Play)
			scene->OnUpdate(dt);
	}

	void EditorApp::OnRender() noexcept
	{
		OPTICK_EVENT("EditorApp::OnRender")
		if (Core::Globals::g_engineIsSuspended)
			return;

		Application::OnRender();
		PRX_ASSERT_MSG(m_sceneManager == nullptr, "Scene Manager is nullptr");
		auto scene = m_sceneManager->GetActiveScene();
		if (scene == nullptr)
			return;

		
		scene->OnRender(m_editorCam);
	}

	void EditorApp::OnUI() noexcept
	{
		OPTICK_EVENT("EditorApp::OnUi")
		Application::OnUI();

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		DrawImGuiMenuBar();
		DrawImGuiProjectWindow();

		std::for_each(m_editorPanels.begin(), m_editorPanels.end(), [](auto panel) { panel->DrawPanel(); });
		
		DrawImGuiAppTimeInfo();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	void EditorApp::OnShutdown() noexcept
	{
		OPTICK_EVENT("EditorApp::OnShutdown")
		PRX_LOG_DEBUG("Begin editor shutdown");

		// Shutdown imgui
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		// Delete all panels
		for (auto& panel : m_editorPanels)
		{
			SAFE_DELETE(panel);
		}
		Application::OnShutdown();
	}

	void EditorApp::CloseEditor()
	{
		PRX_LOG_INFO("Editor close requested");
		m_appWantsExit = true;
	}

	
	void EditorApp::SetupImGui()
	{
		OPTICK_EVENT("EditorApp::SetupImgGui")
		auto d3d = PRX_RESOLVE(Graphics::D3DManager);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui_ImplWin32_Init(m_hWnd);
		ImGui_ImplDX11_Init(d3d->GetDevice(), d3d->GetContext());

		ImFontConfig fontConfig;
		fontConfig.OversampleH = 2;
		fontConfig.OversampleV = 2;
		fontConfig.PixelSnapH = true;
		io.Fonts->AddFontFromFileTTF("Resources/Fonts/JetBrainsMono-Regular.ttf", 16, &fontConfig);

		SetImGuiStyleDracula();
	}

	void EditorApp::DrawImGuiMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// ----- Quit App -----
				if (ImGui::MenuItem("Quit"))
					CloseEditor();

				// ----- Editor Styling -----
				if (ImGui::BeginMenu("Theme Style"))
				{
					if (ImGui::MenuItem("Dracula"))
						SetImGuiStyleDracula();

					if (ImGui::MenuItem("Deep Dark"))
						SetImGuiStyleDeepDark();

					if (ImGui::MenuItem("Visual Studio"))
						SetImGuiStyleVS();

					if (ImGui::MenuItem("Red"))
						SetImGuiStyleRed();

					if (ImGui::MenuItem("Blue Gray"))
						SetImGuiStyleBlueGray();

					if (ImGui::MenuItem("Light Gray"))
						SetImGuiStyleLightDark();

					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::MenuItem(m_showAppStatsWindow ? "Hide Stats Window" : "Show Stats Window"))
					m_showAppStatsWindow = !m_showAppStatsWindow;

				ImGui::EndMenu();
			}


			if (m_sceneManager->GetActiveScene() != nullptr)
			{
				switch (m_sceneState)
				{
				case Proximity::Editor::EditorApp::SceneState::Edit:
					if (ImGui::SmallButton("Play"))
					{
						OnScenePlay();
					}
					break;
				case Proximity::Editor::EditorApp::SceneState::Play:
					if (ImGui::SmallButton("Stop"))
					{
						OnSceneStop();
					}
					break;
				}
			}

			ImGui::EndMainMenuBar();
		}
	}

	void EditorApp::DrawImGuiProjectWindow()
	{
		if (m_isWorkingDirectorySet)
			return;
		else
			ImGui::OpenPopup("Project");

		static char projectName[50] = "ProximityProject";

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		auto modalFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;

		if (ImGui::BeginPopupModal("Project", NULL, modalFlags))
		{
			if (ImGui::Button("New Project"))
				ImGui::OpenPopup("New Project Dialog");

			ImGui::SameLine();

			if (ImGui::Button("Open Existing"))
			{
				auto path = Utils::DirectoryManager::OpenDirFromExplorer("New Project - Choose Directory");
				
				if (!path.empty())
				{
					auto success = OpenProjectDirectory(path);
					// Returns boolean on finding project only
					if (success.has_value())
					{
						OpenProject();
					}
					else
					{
						std::string msg = "Invalid folder or project file is missing!\nChoose a different directory\n";
						msg.append("Selected path: ").append(path);
						MSG_BOX(msg.c_str(), "Failed to open project");
					}
				}
				
			}

			// New project dialog
			bool open = true;
			if (ImGui::BeginPopupModal("New Project Dialog", &open, modalFlags))
			{
				ImGui::InputText("Project Name##inputfield", projectName, 20, ImGuiInputTextFlags_CharsNoBlank);				

				if (ImGui::Button("Create Project"))
				{
					FilePath filepath = Utils::DirectoryManager::OpenDirFromExplorer("New Project - Choose Directory");
					if (!filepath.empty())
					{
						CreateProject(filepath, projectName);
					}
				}

				ImGui::SameLine();

				if (ImGui::Button("Back##CreateProject"))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

			
			ImGui::EndPopup();
		}

	}

	void EditorApp::DrawImGuiAppTimeInfo()
	{
		if (!m_showAppStatsWindow)
			return;

		auto info = GetAppTimeInfo();

		ImGui::Begin("App Stats");
		ImGui::Text(" %.2fms\tFrame time", info.FrameTime * 1000.0f);
		ImGui::Text(" %.2fms\tUpdate time", info.UpdateTime * 1000.0f);
		ImGui::Text(" %.2fms\tRender time", info.RenderTime * 1000.0f);


		ImGui::End();
	}

	
	void EditorApp::OnScenePlay()
	{
		m_sceneState = SceneState::Play;
		Core::Globals::g_editorIsPlaying = true;
		ImGui::SetWindowFocus("Viewport");
		
		auto scene = m_sceneManager->GetActiveScene();
		if (scene)
		{
			Modules::SceneSerializer serializer(scene);
			serializer.Serialize();
			scene->OnScenePlay();
		}
	}

	void EditorApp::OnSceneStop()
	{
		m_sceneState = SceneState::Edit; 
		Core::Globals::g_editorIsPlaying = false;
	
		auto scene = m_sceneManager->GetActiveScene();
		
		if (scene)
		{
			scene->OnSceneStop(); 
			m_sceneManager->LoadScene(scene->GetFilePath().generic_string());
		}
	}

	void EditorApp::CreateProject(const FilePath& filepath, std::string_view projectName)
	{
		m_workingDirectory = filepath / projectName.data();
		m_isWorkingDirectorySet = true;

		CreateProjectDirectory(projectName.data());

		PRX_RESOLVE(Modules::TextureLibrary)->InitProjectLib();
		PRX_RESOLVE(Modules::MaterialLibrary)->InitProjectLib();
		PRX_RESOLVE(Modules::AudioLibrary)->InitProjectLib();
		PRX_RESOLVE(Modules::ScriptLibrary)->InitProjectLib();
		m_sceneManager->InitProjectLib();
	}

	void EditorApp::OpenProject()
	{
		m_isWorkingDirectorySet = true;
		PRX_RESOLVE(Modules::TextureLibrary)->InitProjectLib();
		PRX_RESOLVE(Modules::MaterialLibrary)->InitProjectLib();
		PRX_RESOLVE(Modules::AudioLibrary)->InitProjectLib();
		PRX_RESOLVE(Modules::ScriptLibrary)->InitProjectLib();
		m_sceneManager->InitProjectLib();
	}
	
#pragma region ImGui Styles
	void EditorApp::SetImGuiStyleDeepDark()
	{
		// Deep Dark style by janekb04 from ImThemes
		ImVec4* colors                         = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg]              = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg]               = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border]                = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg]               = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg]               = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive]         = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark]             = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab]            = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header]                = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_HeaderHovered]         = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive]          = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip]            = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab]                   = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered]            = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive]             = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		colors[ImGuiCol_TabUnfocused]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_DockingPreview]        = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg]        = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLines]             = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderLight]      = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget]        = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

		ImGuiStyle& style       = ImGui::GetStyle();
		style.WindowPadding     = ImVec2(8.00f, 8.00f);
		style.FramePadding      = ImVec2(5.00f, 2.00f);
		style.CellPadding       = ImVec2(6.00f, 6.00f);
		style.ItemSpacing       = ImVec2(6.00f, 6.00f);
		style.ItemInnerSpacing  = ImVec2(6.00f, 6.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing     = 25;
		style.ScrollbarSize     = 15;
		style.GrabMinSize       = 10;
		style.WindowBorderSize  = 1;
		style.ChildBorderSize   = 1;
		style.PopupBorderSize   = 1;
		style.FrameBorderSize   = 1;
		style.TabBorderSize     = 1;
		style.WindowRounding    = 7;
		style.ChildRounding     = 4;
		style.FrameRounding     = 3;
		style.PopupRounding     = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding      = 3;
		style.LogSliderDeadzone = 4;
		style.TabRounding       = 4;
	}
	void EditorApp::SetImGuiStyleVS()
	{
		// Rounded Visual Studio style by RedNicStone from ImThemes
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha                     = 1.0f;
		style.DisabledAlpha             = 0.6000000238418579f;
		style.WindowPadding             = ImVec2(8.0f, 8.0f);
		style.WindowRounding            = 4.0f;
		style.WindowBorderSize          = 0.0f;
		style.WindowMinSize             = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign          = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition  = ImGuiDir_Left;
		style.ChildRounding             = 0.0f;
		style.ChildBorderSize           = 1.0f;
		style.PopupRounding             = 4.0f;
		style.PopupBorderSize           = 1.0f;
		style.FramePadding              = ImVec2(4.0f, 3.0f);
		style.FrameRounding             = 2.5f;
		style.FrameBorderSize           = 0.0f;
		style.ItemSpacing               = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing          = ImVec2(4.0f, 4.0f);
		style.CellPadding               = ImVec2(4.0f, 2.0f);
		style.IndentSpacing             = 21.0f;
		style.ColumnsMinSpacing         = 6.0f;
		style.ScrollbarSize             = 11.0f;
		style.ScrollbarRounding         = 2.5f;
		style.GrabMinSize               = 10.0f;
		style.GrabRounding              = 2.0f;
		style.TabRounding               = 3.5f;
		style.TabBorderSize             = 0.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition       = ImGuiDir_Right;
		style.ButtonTextAlign           = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign       = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text]                  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.5921568870544434f, 0.5921568870544434f, 0.5921568870544434f, 1.0f);
		style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_Border]                = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
		style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
		style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
		style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.321568638086319f, 0.321568638086319f, 0.3333333432674408f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3725490272045135f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3725490272045135f, 1.0f);
		style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		style.Colors[ImGuiCol_Button]                = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
		style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		style.Colors[ImGuiCol_Header]                = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
		style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		style.Colors[ImGuiCol_Separator]             = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
		style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
		style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
		style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
		style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.321568638086319f, 0.321568638086319f, 0.3333333432674408f, 1.0f);
		style.Colors[ImGuiCol_Tab]                   = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		style.Colors[ImGuiCol_TabActive]             = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
		style.Colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_TableBorderLight]      = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
		style.Colors[ImGuiCol_TableRowBg]            = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
		style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
		style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_NavHighlight]          = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
		style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
		style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
	}
	void EditorApp::SetImGuiStyleRed()
	{
		// Sonic Riders style by Sewer56 from ImThemes
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha                     = 1.0f;
		style.DisabledAlpha             = 0.6000000238418579f;
		style.WindowPadding             = ImVec2(8.0f, 8.0f);
		style.WindowRounding            = 0.0f;
		style.WindowBorderSize          = 0.0f;
		style.WindowMinSize             = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign          = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition  = ImGuiDir_Left;
		style.ChildRounding             = 0.0f;
		style.ChildBorderSize           = 1.0f;
		style.PopupRounding             = 0.0f;
		style.PopupBorderSize           = 0.0f;
		style.FramePadding              = ImVec2(4.0f, 3.0f);
		style.FrameRounding             = 4.0f;
		style.FrameBorderSize           = 0.0f;
		style.ItemSpacing               = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing          = ImVec2(4.0f, 4.0f);
		style.CellPadding               = ImVec2(4.0f, 2.0f);
		style.IndentSpacing             = 21.0f;
		style.ColumnsMinSpacing         = 6.0f;
		style.ScrollbarSize             = 14.0f;
		style.ScrollbarRounding         = 9.0f;
		style.GrabMinSize               = 10.0f;
		style.GrabRounding              = 4.0f;
		style.TabRounding               = 4.0f;
		style.TabBorderSize             = 0.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition       = ImGuiDir_Right;
		style.ButtonTextAlign           = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign       = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text]                  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.729411780834198f, 0.7490196228027344f, 0.7372549176216125f, 1.0f);
		style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.08627451211214066f, 0.08627451211214066f, 0.08627451211214066f, 0.9399999976158142f);
		style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
		style.Colors[ImGuiCol_Border]                = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 0.5f);
		style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.7098039388656616f, 0.3882353007793427f, 0.3882353007793427f, 0.5400000214576721f);
		style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.8392156958580017f, 0.658823549747467f, 0.658823549747467f, 0.4000000059604645f);
		style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.8392156958580017f, 0.658823549747467f, 0.658823549747467f, 0.6700000166893005f);
		style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.4666666686534882f, 0.2196078449487686f, 0.2196078449487686f, 0.6700000166893005f);
		style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.4666666686534882f, 0.2196078449487686f, 0.2196078449487686f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.4666666686534882f, 0.2196078449487686f, 0.2196078449487686f, 0.6700000166893005f);
		style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.3372549116611481f, 0.1568627506494522f, 0.1568627506494522f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
		style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
		style.Colors[ImGuiCol_CheckMark]             = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.7098039388656616f, 0.3882353007793427f, 0.3882353007793427f, 1.0f);
		style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.8392156958580017f, 0.658823549747467f, 0.658823549747467f, 1.0f);
		style.Colors[ImGuiCol_Button]                = ImVec4(0.4666666686534882f, 0.2196078449487686f, 0.2196078449487686f, 0.6499999761581421f);
		style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.7098039388656616f, 0.3882353007793427f, 0.3882353007793427f, 0.6499999761581421f);
		style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 0.5f);
		style.Colors[ImGuiCol_Header]                = ImVec4(0.7098039388656616f, 0.3882353007793427f, 0.3882353007793427f, 0.5400000214576721f);
		style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.8392156958580017f, 0.658823549747467f, 0.658823549747467f, 0.6499999761581421f);
		style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.8392156958580017f, 0.658823549747467f, 0.658823549747467f, 0.0f);
		style.Colors[ImGuiCol_Separator]             = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
		style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.7098039388656616f, 0.3882353007793427f, 0.3882353007793427f, 0.5400000214576721f);
		style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.7098039388656616f, 0.3882353007793427f, 0.3882353007793427f, 0.5400000214576721f);
		style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.7098039388656616f, 0.3882353007793427f, 0.3882353007793427f, 0.5400000214576721f);
		style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.8392156958580017f, 0.658823549747467f, 0.658823549747467f, 0.6600000262260437f);
		style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.8392156958580017f, 0.658823549747467f, 0.658823549747467f, 0.6600000262260437f);
		style.Colors[ImGuiCol_Tab]                   = ImVec4(0.7098039388656616f, 0.3882353007793427f, 0.3882353007793427f, 0.5400000214576721f);
		style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.8392156958580017f, 0.658823549747467f, 0.658823549747467f, 0.6600000262260437f);
		style.Colors[ImGuiCol_TabActive]             = ImVec4(0.8392156958580017f, 0.658823549747467f, 0.658823549747467f, 0.6600000262260437f);
		style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.06666667014360428f, 0.09803921729326248f, 0.1490196138620377f, 0.9700000286102295f);
		style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.1372549086809158f, 0.2588235437870026f, 0.4196078479290009f, 1.0f);
		style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
		style.Colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_TableBorderLight]      = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
		style.Colors[ImGuiCol_TableRowBg]            = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
		style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
		style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
		style.Colors[ImGuiCol_NavHighlight]          = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
		style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
		style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
	}
	void EditorApp::SetImGuiStyleBlueGray()
	{
		// Dark Ruda style by Raikiri from ImThemes
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha                     = 1.0f;
		style.DisabledAlpha             = 0.6000000238418579f;
		style.WindowPadding             = ImVec2(8.0f, 8.0f);
		style.WindowRounding            = 0.0f;
		style.WindowBorderSize          = 1.0f;
		style.WindowMinSize             = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign          = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition  = ImGuiDir_Left;
		style.ChildRounding             = 0.0f;
		style.ChildBorderSize           = 1.0f;
		style.PopupRounding             = 0.0f;
		style.PopupBorderSize           = 1.0f;
		style.FramePadding              = ImVec2(4.0f, 3.0f);
		style.FrameRounding             = 4.0f;
		style.FrameBorderSize           = 0.0f;
		style.ItemSpacing               = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing          = ImVec2(4.0f, 4.0f);
		style.CellPadding               = ImVec2(4.0f, 2.0f);
		style.IndentSpacing             = 21.0f;
		style.ColumnsMinSpacing         = 6.0f;
		style.ScrollbarSize             = 14.0f;
		style.ScrollbarRounding         = 9.0f;
		style.GrabMinSize               = 10.0f;
		style.GrabRounding              = 4.0f;
		style.TabRounding               = 4.0f;
		style.TabBorderSize             = 0.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition       = ImGuiDir_Right;
		style.ButtonTextAlign           = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign       = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text]                  = ImVec4(0.9490196108818054f, 0.95686274766922f, 0.9764705896377563f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.3568627536296844f, 0.4196078479290009f, 0.4666666686534882f, 1.0f);
		style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.1098039224743843f, 0.1490196138620377f, 0.168627455830574f, 1.0f);
		style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.1490196138620377f, 0.1764705926179886f, 0.2196078449487686f, 1.0f);
		style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
		style.Colors[ImGuiCol_Border]                = ImVec4(0.0784313753247261f, 0.09803921729326248f, 0.1176470592617989f, 1.0f);
		style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.2000000029802322f, 0.2470588237047195f, 0.2862745225429535f, 1.0f);
		style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.1176470592617989f, 0.2000000029802322f, 0.2784313857555389f, 1.0f);
		style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.08627451211214066f, 0.1176470592617989f, 0.1372549086809158f, 1.0f);
		style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.08627451211214066f, 0.1176470592617989f, 0.1372549086809158f, 0.6499999761581421f);
		style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.0784313753247261f, 0.09803921729326248f, 0.1176470592617989f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
		style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.1490196138620377f, 0.1764705926179886f, 0.2196078449487686f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.3899999856948853f);
		style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.2000000029802322f, 0.2470588237047195f, 0.2862745225429535f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.1764705926179886f, 0.2196078449487686f, 0.2470588237047195f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.08627451211214066f, 0.2078431397676468f, 0.3098039329051971f, 1.0f);
		style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.2784313857555389f, 0.5568627715110779f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.2784313857555389f, 0.5568627715110779f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.3686274588108063f, 0.6078431606292725f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_Button]                = ImVec4(0.2000000029802322f, 0.2470588237047195f, 0.2862745225429535f, 1.0f);
		style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.2784313857555389f, 0.5568627715110779f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.05882352963089943f, 0.529411792755127f, 0.9764705896377563f, 1.0f);
		style.Colors[ImGuiCol_Header]                = ImVec4(0.2000000029802322f, 0.2470588237047195f, 0.2862745225429535f, 0.550000011920929f);
		style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.800000011920929f);
		style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
		style.Colors[ImGuiCol_Separator]             = ImVec4(0.2000000029802322f, 0.2470588237047195f, 0.2862745225429535f, 1.0f);
		style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 0.7799999713897705f);
		style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 1.0f);
		style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.25f);
		style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.6700000166893005f);
		style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.949999988079071f);
		style.Colors[ImGuiCol_Tab]                   = ImVec4(0.1098039224743843f, 0.1490196138620377f, 0.168627455830574f, 1.0f);
		style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.800000011920929f);
		style.Colors[ImGuiCol_TabActive]             = ImVec4(0.2000000029802322f, 0.2470588237047195f, 0.2862745225429535f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.1098039224743843f, 0.1490196138620377f, 0.168627455830574f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.1098039224743843f, 0.1490196138620377f, 0.168627455830574f, 1.0f);
		style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
		style.Colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_TableBorderLight]      = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
		style.Colors[ImGuiCol_TableRowBg]            = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
		style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
		style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
		style.Colors[ImGuiCol_NavHighlight]          = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
		style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
		style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
	}
	void EditorApp::SetImGuiStyleLightDark()
	{
		// Photoshop style by Derydoca from ImThemes
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha                     = 1.0f;
		style.DisabledAlpha             = 0.6000000238418579f;
		style.WindowPadding             = ImVec2(8.0f, 8.0f);
		style.WindowRounding            = 4.0f;
		style.WindowBorderSize          = 1.0f;
		style.WindowMinSize             = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign          = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition  = ImGuiDir_Left;
		style.ChildRounding             = 4.0f;
		style.ChildBorderSize           = 1.0f;
		style.PopupRounding             = 2.0f;
		style.PopupBorderSize           = 1.0f;
		style.FramePadding              = ImVec2(4.0f, 3.0f);
		style.FrameRounding             = 2.0f;
		style.FrameBorderSize           = 1.0f;
		style.ItemSpacing               = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing          = ImVec2(4.0f, 4.0f);
		style.CellPadding               = ImVec2(4.0f, 2.0f);
		style.IndentSpacing             = 21.0f;
		style.ColumnsMinSpacing         = 6.0f;
		style.ScrollbarSize             = 13.0f;
		style.ScrollbarRounding         = 12.0f;
		style.GrabMinSize               = 7.0f;
		style.GrabRounding              = 0.0f;
		style.TabRounding               = 0.0f;
		style.TabBorderSize             = 1.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition       = ImGuiDir_Right;
		style.ButtonTextAlign           = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign       = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text]                  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
		style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 1.0f);
		style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.0f);
		style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
		style.Colors[ImGuiCol_Border]                = ImVec4(0.2627451121807098f, 0.2627451121807098f, 0.2627451121807098f, 1.0f);
		style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.1568627506494522f, 0.1568627506494522f, 0.1568627506494522f, 1.0f);
		style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 1.0f);
		style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 1.0f);
		style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1450980454683304f, 1.0f);
		style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.1921568661928177f, 0.1921568661928177f, 0.1921568661928177f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.1568627506494522f, 0.1568627506494522f, 0.1568627506494522f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.2745098173618317f, 0.2745098173618317f, 0.2745098173618317f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.2980392277240753f, 0.2980392277240753f, 0.2980392277240753f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_CheckMark]             = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.3882353007793427f, 0.3882353007793427f, 0.3882353007793427f, 1.0f);
		style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_Button]                = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
		style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(1.0f, 1.0f, 1.0f, 0.1560000032186508f);
		style.Colors[ImGuiCol_ButtonActive]          = ImVec4(1.0f, 1.0f, 1.0f, 0.3910000026226044f);
		style.Colors[ImGuiCol_Header]                = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
		style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 1.0f);
		style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 1.0f);
		style.Colors[ImGuiCol_Separator]             = ImVec4(0.2627451121807098f, 0.2627451121807098f, 0.2627451121807098f, 1.0f);
		style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.3882353007793427f, 0.3882353007793427f, 0.3882353007793427f, 1.0f);
		style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.0f, 1.0f, 1.0f, 0.25f);
		style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(1.0f, 1.0f, 1.0f, 0.6700000166893005f);
		style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_Tab]                   = ImVec4(0.09411764889955521f, 0.09411764889955521f, 0.09411764889955521f, 1.0f);
		style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.3490196168422699f, 0.3490196168422699f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_TabActive]             = ImVec4(0.1921568661928177f, 0.1921568661928177f, 0.1921568661928177f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.09411764889955521f, 0.09411764889955521f, 0.09411764889955521f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.1921568661928177f, 0.1921568661928177f, 0.1921568661928177f, 1.0f);
		style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.5843137502670288f, 0.5843137502670288f, 0.5843137502670288f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
		style.Colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
		style.Colors[ImGuiCol_TableBorderLight]      = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
		style.Colors[ImGuiCol_TableRowBg]            = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
		style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(1.0f, 1.0f, 1.0f, 0.1560000032186508f);
		style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_NavHighlight]          = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 0.3882353007793427f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.0f, 0.0f, 0.0f, 0.5860000252723694f);
		style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.0f, 0.0f, 0.0f, 0.5860000252723694f);
	}
	void EditorApp::SetImGuiStyleDracula()
	{
		auto& colors = ImGui::GetStyle().Colors;

		colors[ImGuiCol_WindowBg]             = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
		colors[ImGuiCol_MenuBarBg]            = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

		// Border
		colors[ImGuiCol_Border]               = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
		colors[ImGuiCol_BorderShadow]         = ImVec4{ 0.0f, 0.0f, 0.0f, 0.24f };

		// Text
		colors[ImGuiCol_Text]                 = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
		colors[ImGuiCol_TextDisabled]         = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };

		// Headers
		colors[ImGuiCol_Header]               = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
		colors[ImGuiCol_HeaderHovered]        = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
		colors[ImGuiCol_HeaderActive]         = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button]               = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
		colors[ImGuiCol_ButtonHovered]        = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
		colors[ImGuiCol_ButtonActive]         = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
		colors[ImGuiCol_CheckMark]            = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };

		// Popups
		colors[ImGuiCol_PopupBg]              = ImVec4{ 0.1f, 0.1f, 0.13f, 0.92f };

		// Slider
		colors[ImGuiCol_SliderGrab]           = ImVec4{ 0.44f, 0.37f, 0.61f, 0.54f };
		colors[ImGuiCol_SliderGrabActive]     = ImVec4{ 0.74f, 0.58f, 0.98f, 0.54f };

		// Frame BG
		colors[ImGuiCol_FrameBg]              = ImVec4{ 0.13f, 0.13f, 0.17f, 1.0f };
		colors[ImGuiCol_FrameBgHovered]       = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
		colors[ImGuiCol_FrameBgActive]        = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab]                  = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
		colors[ImGuiCol_TabHovered]           = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };
		colors[ImGuiCol_TabActive]            = ImVec4{ 0.2f, 0.22f, 0.27f, 1.0f };
		colors[ImGuiCol_TabUnfocused]         = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive]   = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg]              = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
		colors[ImGuiCol_TitleBgActive]        = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed]     = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

		// Scrollbar
		colors[ImGuiCol_ScrollbarBg]          = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
		colors[ImGuiCol_ScrollbarGrab]        = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
		colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };

		// Seperator
		colors[ImGuiCol_Separator]            = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };
		colors[ImGuiCol_SeparatorHovered]     = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };
		colors[ImGuiCol_SeparatorActive]      = ImVec4{ 0.84f, 0.58f, 1.0f, 1.0f };

		// Resize Grip
		colors[ImGuiCol_ResizeGrip]           = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
		colors[ImGuiCol_ResizeGripHovered]    = ImVec4{ 0.74f, 0.58f, 0.98f, 0.29f };
		colors[ImGuiCol_ResizeGripActive]     = ImVec4{ 0.84f, 0.58f, 1.0f, 0.29f };

		// Docking
		colors[ImGuiCol_DockingPreview]       = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };

		auto& style             = ImGui::GetStyle();
		style.TabRounding       = 4;
		style.ScrollbarRounding = 9;
		style.WindowRounding    = 7;
		style.GrabRounding      = 3;
		style.FrameRounding     = 3;
		style.PopupRounding     = 4;
		style.ChildRounding     = 4;
	}
#pragma endregion
}