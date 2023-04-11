#include "editorpch.h"
#include "Editor/Panels/BrowserPanel.h"
#include "Engine/Modules/SceneSerializer.h"

namespace Proximity::Editor::Panels
{
	BrowserPanel::BrowserPanel()
		:
		EditorPanel("Content Browswer")
	{
		m_shaderLib    = PRX_RESOLVE(Modules::ShaderLibrary);
		m_materialLib  = PRX_RESOLVE(Modules::MaterialLibrary);
		m_textureLib   = PRX_RESOLVE(Modules::TextureLibrary);
		m_audioLibrary = PRX_RESOLVE(Modules::AudioLibrary);
		m_sceneManager = PRX_RESOLVE(Core::SceneManager);
	}

	void BrowserPanel::Draw()
	{
		if (ImGui::Button("Save All"))
		{
			Utils::Timer timer;
			timer.Reset(); timer.Start();

			Modules::SceneSerializer serializer(m_sceneManager->GetActiveScene());
			serializer.Serialize();
			m_materialLib->SerializeMaterials();
			m_shaderLib->SerializeShaders();
			// TODO: Serialize audio data...?
			timer.Stop();
			
			PRX_LOG_INFO("Serialized scene in %fms", timer.TotalTime() * 1000.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Read Scene"))
		{
			try
			{
				Modules::SceneSerializer serializer(m_sceneManager->GetActiveScene());
				serializer.Deserialize("Test/scene.prx");
			}
			catch (const Proximity::Execptions::MethodNotImplemented& e)
			{
				PRX_LOG_ERROR("Message: %s", e.what());
			}

			
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

			ImGui::Separator();

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

		static std::string vs, ps;

		if (ImGui::BeginPopupModal("Material Wizard", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
		{
			auto& shaderMap = m_shaderLib->GetMap();

			static char matName[20] = "UntitledMat";
			ImGui::InputText("Material Name##inputfield", matName, 20);

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();
			
			// Select vertex shader
			if (ImGui::BeginCombo("##MaterialWizardVS", vs.empty() ? "Choose Vertex Shader" : vs.c_str()))
			{
				for (auto& pair : shaderMap)
				{
					if (pair.second->GetType() == Graphics::GPUShaderType::Vertex)
					{
						if (ImGui::Selectable(pair.first.c_str()))
							vs = pair.first;
					}
				}

				ImGui::EndCombo();
			}

			ImGui::SameLine();

			// Select pixel shader
			if (ImGui::BeginCombo("##MaterialWizardPS", ps.empty() ? "Choose Pixel Shader" : ps.c_str()))
			{
				for (const auto& pair : shaderMap)
				{
					if (pair.second->GetType() == Graphics::GPUShaderType::Pixel)
					{
						if (ImGui::Selectable(pair.first.c_str()))
							ps = pair.first;
					}
				}

				ImGui::EndCombo();
			}



			if (ImGui::Button("Cancel##Failed to create material"))
			{
				vs.clear();
				ps.clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();


			if (m_materialLib->Exists(matName))
			{
				ImGui::TextColored({ 1, 1, 0, 1 }, "Material with the same name exists");
			}
			else if (!vs.empty() && !ps.empty())
			{
				if (ImGui::Button("Create##Created material"))
				{
					Graphics::Material mat = Graphics::Material(m_shaderLib->Get(vs), m_shaderLib->Get(ps), matName);

					m_materialLib->AddMaterial(std::make_shared<Graphics::Material>(mat));

					vs.clear();
					ps.clear();
					ImGui::CloseCurrentPopup();
				}
			}
			else
			{
				ImGui::TextColored({ 1, 1, 0, 1 }, "Select shaders");
			}


			ImGui::EndPopup();
		}
	}

	static float imageSize = 100.0f;

	void BrowserPanel::DrawTextureLibrary()
	{
		static bool textureSelected;
		if (ImGui::BeginTabItem("Texture Library"))
		{
			if (ImGui::Button("Refresh Library"))
				m_textureLib->Refresh();
			ImGui::SameLine();
			ImGui::DragFloat("Image Size##TextureLibrary", &imageSize, 0.1f, 10.0f, 300.0f, "%.1f",ImGuiSliderFlags_NoInput | ImGuiSliderFlags_AlwaysClamp);
			
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			for (auto& pair : m_textureLib->GetMap())
			{
				const std::string& name = pair.first;

				ImGui::Image((void*)pair.second->SRV.Get(), { imageSize, imageSize});

				ImGui::SameLine();

				if (ImGui::Selectable(name.c_str(), textureSelected, 0))
					m_textureLib->UpdateSelected(name);
			}
			
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

			ImGui::Separator();

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
		static bool audioFileSelected = false;
		if (ImGui::BeginTabItem("Audio Library"))
		{
			if (ImGui::Button("Refresh Library"))
				m_audioLibrary->Refresh();
			
			if (m_audioLibrary->Count() < 1)
			{
				ImGui::TextColored({ 1, 1, 0, 1 }, "AUDIO LIBRARY IS EMPTY!");
				ImGui::EndTabItem();
				return;
			}

			for (auto& pair : m_audioLibrary->GetMap())
			{
				const auto& name = pair.first;
				
				if (ImGui::Selectable(name.c_str(), audioFileSelected, 0))
					m_audioLibrary->OnAudioSelected(name);
			}

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

			ImGui::Separator();
			
			for (auto& pair : m_materialLib->GetMap())
			{
				auto& name = pair.first;
				if (ImGui::Selectable(name.c_str(), materialSelected))
					m_materialLib->UpdateSelected(name);

			}
			
			DrawMaterialWizard();
			
			ImGui::EndTabItem();
		}
	}
}