#include "editorpch.h"
#include "Editor/Panels/BrowserPanel.h"

namespace Proximity::Editor::Panels
{
	BrowserPanel::BrowserPanel()
		:
		EditorPanel("Content Browswer")
	{
		m_shaderLib = Core::Globals::g_engineServices.ResolveService<Modules::ShaderLibrary>();
		m_sceneManager = Core::Globals::g_engineServices.ResolveService<Core::SceneManager>();
	}

	void BrowserPanel::Draw()
	{
		if (ImGui::Button("Save All"))
		{
			PRX_LOG_INFO("Save all button pressed");
		}

		if (ImGui::BeginTabBar("Content Browser Tabs", ImGuiTabBarFlags_Reorderable))
		{
			DrawSceneLibrary();
			DrawTextureLibrary();
			DrawShaderLibrary();
			DrawAudioibrary();

			ImGui::EndTabBar();
		}
	}

	void BrowserPanel::DrawSceneLibrary()
	{
		if (ImGui::BeginTabItem("Scene Library"))
		{
			if (ImGui::Button("Create Scene"))
				ImGui::OpenPopup("Scene Wizard");

			DrawSceneList();
			DrawSceneWizard();

			ImGui::EndTabItem();
		}
	}

	void BrowserPanel::DrawSceneList()
	{
		static bool sceneSelected = false;
		for (auto& scenePath : m_sceneManager->GetScenePathList())
		{
			auto name = Utils::DirectoryManager::GetFileNameFromDir(scenePath, false);
			
			if (ImGui::Selectable(name.c_str(), sceneSelected, ImGuiSelectableFlags_AllowDoubleClick))
			{
				if (ImGui::IsMouseDoubleClicked(0))
				{
					m_sceneManager->LoadScene(name);
				}
			}
		}
	}

	void BrowserPanel::DrawSceneWizard()
	{
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));


		// ----- Scene Creation Wizard -----
		if (ImGui::BeginPopupModal("Scene Wizard", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Create scene here!");

			static char sceneName[50] = "UntitledScene";
			bool create = ImGui::InputText("Scene name##inputfield", sceneName, 20, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);

			if (ImGui::Button("Create##scene") || create)
			{
				bool created = m_sceneManager->CreateScene(sceneName);

				if (created)
					ImGui::CloseCurrentPopup();
				else
					ImGui::OpenPopup("Failed to create scene");
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel##scene"))
				ImGui::CloseCurrentPopup();

			// ----- Scene creation failure -----
			if (ImGui::BeginPopupModal("Failed to create scene", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
			{
				ImGui::TextColored({ 1,1,0,1 }, "Scene with the same name already exists!");

				if (ImGui::Button("Okay##Faile to create scene"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::EndPopup();
		}
	}

	void BrowserPanel::DrawTextureLibrary()
	{
		if (ImGui::BeginTabItem("Texture Library"))
		{
			ImGui::TextColored({ 1, 1, 0, 1 }, "TEXTURE LIBRARY IS EMPTY!");
			ImGui::EndTabItem();
			return;

			ImGui::EndTabItem();
		}
	}

	void BrowserPanel::DrawShaderLibrary()
	{
		static bool shaderSelected = false;
		if (m_shaderLib == nullptr)
		{
			ImGui::TextColored({ 1, 0, 0, 1 }, "Failed to get shader library");
			return;
		}

		
		if (ImGui::BeginTabItem("Shader Library"))
		{
			if (ImGui::Button("Hot Reload All Shaders"))
			{
				auto msg = m_shaderLib->HotReloadAll();
				PRX_LOG_INFO("Hot reload info: %s", msg.c_str());
			}

			auto size = m_shaderLib->Count();
			if (size == 0)
			{
				ImGui::TextColored({ 1, 1, 0, 1 }, "SHADER LIBRARY IS EMPTY!");
				ImGui::EndTabItem();
				return;
			}
			else
			{
				for (auto& pair : m_shaderLib->GetMap())
				{
					auto& name = pair.first;
					if (ImGui::Selectable(name.c_str(), shaderSelected))
						m_shaderLib->UpdateSelected(name);
					
				}
			}


			ImGui::EndTabItem();
		}
	}

	void BrowserPanel::DrawAudioibrary()
	{
		if (ImGui::BeginTabItem("Audio Library"))
		{
			ImGui::TextColored({ 1, 1, 0, 1 }, "AUDIO LIBRARY IS EMPTY!");
			ImGui::EndTabItem();
			return;

			ImGui::EndTabItem();
		}
	}
}