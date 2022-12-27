#include "editorpch.h"
#include "Editor/Panels/BrowserPanel.h"

namespace Proximity::Editor::Panels
{
	BrowswerPanel::BrowswerPanel()
		:
		EditorPanel("Content Browswer")
	{
		m_shaderLib = Core::Globals::g_engineServices.ResolveService<Modules::ShaderLibrary>();
	}
	void BrowswerPanel::Draw()
	{
		if (ImGui::Button("Save All"))
		{

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

	void BrowswerPanel::DrawSceneLibrary()
	{
		if (ImGui::BeginTabItem("Scene Library"))
		{
			auto& sceneList = WORLD->GetSceneList();
			for (auto& scene : sceneList)
			{
				ImGui::Text(scene->GetName().c_str());
			}

			if (ImGui::Button("Create Scene"))
				ImGui::OpenPopup("Scene Wizard");

			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal("Scene Wizard", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Create scene here!");

				static char sceneName[20];
				ImGui::InputText("Scene name##inputfield", sceneName, 20, ImGuiInputTextFlags_CharsNoBlank);

				if (ImGui::Button("Create##scene"))
				{
					auto id = (Math::U32)WORLD->CreateScene(sceneName);
					WORLD->SetScene(id);
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel##scene"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}


			ImGui::EndTabItem();
		}
	}

	void BrowswerPanel::DrawTextureLibrary()
	{
		if (ImGui::BeginTabItem("Texture Library"))
		{
			ImGui::TextColored({ 1, 1, 0, 1 }, "TEXTURE LIBRARY IS EMPTY!");
			ImGui::EndTabItem();
			return;

			ImGui::EndTabItem();
		}
	}

	void BrowswerPanel::DrawShaderLibrary()
	{
		if (m_shaderLib == nullptr)
		{
			ImGui::TextColored({ 1, 0, 0, 1 }, "Failed to get shader library");
			return;
		}

		
		if (ImGui::BeginTabItem("Shader Library"))
		{
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
					ImGui::Text(name.c_str());
				}
			}

			ImGui::EndTabItem();
		}
	}

	void BrowswerPanel::DrawAudioibrary()
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