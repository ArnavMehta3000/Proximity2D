#include "editorpch.h"
#include "Editor/Panels/BrowserPanel.h"

namespace Proximity::Editor::Panels
{
	BrowserPanel::BrowserPanel()
		:
		EditorPanel("Content Browswer")
	{
		m_shaderLib    = PRX_RESOLVE(Modules::ShaderLibrary);
		m_materialLib  = PRX_RESOLVE(Modules::MaterialLibrary);
		m_sceneManager = PRX_RESOLVE(Core::SceneManager);
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
			DrawMaterialLibrary();
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

			static char sceneName[20] = "UntitledScene";
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

				if (ImGui::Button("Okay##Failed to create scene"))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::EndPopup();
		}
	}

	void BrowserPanel::DrawMaterialWizard()
	{
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		static std::string chosen1, chosen2;

		if (ImGui::BeginPopupModal("Material Wizard", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
		{
			auto& shaderMap = m_shaderLib->GetMap();

			static char matName[20] = "UntitledMat";
			ImGui::InputText("Material Name##inputfield", matName, 20);

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();
			

			if (ImGui::BeginCombo("Pixel Shader##MaterialWizard", chosen1.empty() ? "Choose Vertex Shader" : chosen1.c_str(), ImGuiComboFlags_PopupAlignLeft))
			{
				for (auto& pair : shaderMap)
				{
					if (pair.second->GetType() == Graphics::GPUShaderType::Pixel)
					{
						if (ImGui::Selectable(pair.first.c_str()))
						{
							chosen1 = pair.first;
						}
					}
				}

				ImGui::EndCombo();
			}
			
			if (ImGui::BeginCombo("Vertex Shader##MaterialWizard", chosen2.empty() ? "Choose Pixl Shader" : chosen2.c_str(), ImGuiComboFlags_PopupAlignLeft))
			{
				for (const auto& pair : shaderMap)
				{
					if (pair.second->GetType() == Graphics::GPUShaderType::Vertex)
					{
						if (ImGui::Selectable(pair.first.c_str()))
						{
							chosen2 = pair.first;
						}
					}
				}

				ImGui::EndCombo();
			}



			if (ImGui::Button("Cancel##Failed to create material"))
			{
				chosen1.clear();
				chosen2.clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			
			bool exists = m_materialLib->Exists(matName);
			if (!chosen1.empty() && !exists)  // Only allow creation of material if slot 1 is full and name is not used
			{
				if (ImGui::Button("Create##Created material"))
				{


					Graphics::Material mat;
					mat = Graphics::Material(m_shaderLib->Get(chosen1), m_shaderLib->Get(chosen2), matName);  // Vertex shader present

					m_materialLib->AddMaterial(std::make_shared<Graphics::Material>(mat));

					chosen1.clear();
					chosen2.clear();
					ImGui::CloseCurrentPopup();
				}
			}
			else if (exists)
			{
				ImGui::TextColored({ 1, 1, 0, 1 }, "Material with the same name exists");
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
					// TODO: Make internal shader unmodifiable
					/*if (pair.second->IsInternal())
					{
						ImGui::Selectable(pair.first.c_str(), false, ImGuiSelectableFlags_Disabled);
					}
					else*/
					{
						auto& name = pair.first;
						if (ImGui::Selectable(name.c_str(), shaderSelected))
							m_shaderLib->UpdateSelected(name);
					}
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
	
	void BrowserPanel::DrawMaterialLibrary()
	{
		static bool materialSelected = false;

		if (ImGui::BeginTabItem("Material Library"))
		{
			if (ImGui::Button("Create New Material"))
				ImGui::OpenPopup("Material Wizard");

			auto size = m_materialLib->Count();
			if (size == 0)
			{
				ImGui::TextColored({ 1, 1, 0, 1 }, "MATERIAL LIBRARY IS EMPTY!");
				ImGui::EndTabItem();
				return;
			}
			else
			{
				for (auto& pair : m_materialLib->GetMap())
				{
					auto& name = pair.first;
					if (ImGui::Selectable(name.c_str(), materialSelected))
						m_materialLib->UpdateSelected(name);

				}
			}
			
			DrawMaterialWizard();
			
			ImGui::EndTabItem();
		}
	}
}