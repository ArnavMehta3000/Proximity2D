#include "editorpch.h"
#include "Editor/Panels/BrowserPanel.h"
#include "Engine/Modules/SceneSerializer.h"
#include "optick/include/optick.h"

namespace Proximity::Editor::Panels
{
	BrowserPanel::BrowserPanel()
		:
		EditorPanel("Content Browswer")
	{
		m_shaderLib     = PRX_RESOLVE(Modules::ShaderLibrary);
		m_materialLib   = PRX_RESOLVE(Modules::MaterialLibrary);
		m_textureLib    = PRX_RESOLVE(Modules::TextureLibrary);
		m_audioLibrary  = PRX_RESOLVE(Modules::AudioLibrary);
		m_scriptLibrary = PRX_RESOLVE(Modules::ScriptLibrary);
		m_sceneManager  = PRX_RESOLVE(Core::SceneManager);
	}

	void BrowserPanel::Draw()
	{
		OPTICK_EVENT("BrowserPanel::Draw")
		if (ImGui::Button("Save All"))
		{
			Modules::SceneSerializer serializer(m_sceneManager->GetActiveScene());
			serializer.Serialize();
			m_materialLib->SerializeMaterials();
			m_shaderLib->SerializeShaders();
			// TODO: Serialize audio data...?			
		}
		ImGui::SameLine();
		if (ImGui::Button("Read Scene"))
		{
			try
			{
				Modules::SceneSerializer serializer(m_sceneManager->GetActiveScene());
				auto scenePath = DirectoryManager::s_appDirectories.ScenesPath;
				serializer.Deserialize((scenePath / "UntitledScene.prxscene").string());
			}
			catch (const Proximity::Execptions::MethodNotImplemented& e)
			{
				PRX_LOG_ERROR("Message: %s", e.what());
			}

			
		}

		if (ImGui::BeginTabBar("Content Browser Tabs", ImGuiTabBarFlags_Reorderable))
		{
			DrawSceneLibrary();
			DrawScriptsLibrary();
			DrawTextureLibrary();
			DrawShaderLibrary();
			DrawMaterialLibrary();
			DrawAudioibrary();

			ImGui::EndTabBar();
		}
	}

	void BrowserPanel::DrawScriptsLibrary()
	{
		auto CreateNewLuaFile = [this](const std::string& filename)
		{
			// Create a lua file in the script folder with the provided name
			auto path = Utils::DirectoryManager::s_appDirectories.ScriptsPath.string();
			path.append("\\" + filename);

			std::ofstream luaFile(path);
			luaFile << "-- Proximity Lua script file: " << filename;
			luaFile << "\n\n\n";
			luaFile << "function PRX.OnStart()\nend\n\n";
			luaFile << "function PRX.OnUpdate(dt)\nend\n\n";
			luaFile << "function PRX.OnKeyUp(keyName, isUp, isDown)\nend\n\n";
			luaFile << "function PRX.OnKeyDown(keyName, isUp, isDown)\nend\n\n";
			luaFile << "function PRX.OnMouseInput(keyName)\nend\n\n";
			luaFile << "function PRX.OnCollisionStart(manifold)\nend\n\n";
			luaFile << "function PRX.OnCollisionEnd(manifold)\nend\n\n";
			luaFile.close();


			m_scriptLibrary->AddLuaScript(std::make_shared<Scripting::LuaScript>(path));
		};

		auto ScriptWizard = [&]()
		{
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal("Script Wizard", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
			{
				static char scriptName[20] = "NewScript.lua";
				bool create = ImGui::InputText("Script name##inputfield", scriptName, 20, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);


				// Check if name exists in script map
				if (!m_scriptLibrary->Exists(scriptName))
				{
					if (ImGui::Button("Create##script") || create)
					{
						CreateNewLuaFile(scriptName);
						ImGui::CloseCurrentPopup();
					}
				}
				else
				{
					ImGui::TextColored({ 1, 1 ,0, 1 }, "Script with the same name already exists!");
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel##script"))
					ImGui::CloseCurrentPopup();
				
				ImGui::EndPopup();
			}
		};


		if (ImGui::BeginTabItem("Scripts"))
		{
			if (ImGui::Button("Create Script"))
				ImGui::OpenPopup("Script Wizard");
			
			ImGui::Separator();

			ScriptWizard();
			
			// Iterate over all files in scripts library
			auto& scripts = m_scriptLibrary->GetMap();

			for (auto& pair : scripts)
			{
				if (ImGui::Selectable(pair.first.c_str()))
					m_scriptLibrary->UpdateSelected(pair.first);
			}
			ImGui::EndTabItem();
		}
	}

	void BrowserPanel::DrawSceneLibrary()
	{
		if (ImGui::BeginTabItem("Scenes"))
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
					m_sceneManager->LoadScene(scenePath.string());
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
		if (ImGui::BeginTabItem("Textures"))
		{
			if (ImGui::Button("Refresh Library"))
				m_textureLib->Refresh();
			ImGui::SameLine();
			ImGui::DragFloat("Image Size##TextureLibrary", &imageSize, 0.1f, 10.0f, 300.0f, "%.1f",ImGuiSliderFlags_NoInput | ImGuiSliderFlags_AlwaysClamp);
			
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (m_textureLib->Count() <= 1)
			{
				ImGui::TextColored({ 1, 1, 0, 1 }, "TEXTURE LIBRARY IS EMPTY!");
				ImGui::EndTabItem();
				return;
			}

			for (auto& pair : m_textureLib->GetMap())
			{
				const std::string& name = pair.first;
				if (name == "_InternalTex")
					continue;

				ImGui::Image((void*)pair.second->SRV.Get(), { imageSize, imageSize});

				ImGui::SameLine();

				if (ImGui::Selectable(name.c_str(), textureSelected, 0))
					m_textureLib->UpdateSelected(name);
			}
			
			ImGui::EndTabItem();
		}
	}

	static std::string type = "Choose shader";
	Graphics::GPUShaderType shaderType = Graphics::GPUShaderType::None;
	void BrowserPanel::DrawShaderLibrary()
	{
		static bool shaderSelected = false;

		auto CreateShaderFile = [this](const std::string filename, Graphics::GPUShaderType shaderType)
		{
			// Generate path to shaders
			auto path = Utils::DirectoryManager::s_appDirectories.ShadersPath.string();
			path.append("\\" + filename);

			std::string entryFunc = "";

			std::ofstream file(path);
			switch (shaderType)
			{
			case Proximity::Graphics::GPUShaderType::Vertex:
				file << "struct VSInput\r{\r\tfloat3 Position : POSITION; \r\tfloat2 TexCoord : COLOR; \r}; \r\r";
				file << "struct VSOutput\r{\r\tfloat4 Position : SV_POSITION; \r\tfloat2 TexCoord : COLOR; \r}; \r\r";
				file << "cbuffer CameraMatrices : register(b0)\r{\r    matrix WorldMatrix; \r    matrix ViewMatrix; \r    matrix ProjectionMatrix; \r}; \r\r";
				file << "VSOutput VSmain(VSInput input)\r{\r    VSOutput output; \r    output.Position = float4(input.Position, 1.0f);";
				file << "\r\r    output.Position = mul(output.Position, WorldMatrix); \r    output.Position = mul(output.Position, ViewMatrix); \r    output.Position = mul(output.Position, ProjectionMatrix); \r\r    output.TexCoord = input.TexCoord; \r\r    ";
				file << "return output; \r}";

				entryFunc = "VSmain";
				break;


			case Proximity::Graphics::GPUShaderType::Pixel:
				file << "struct VSInput\r{\r\tfloat3 Position : POSITION;\r\tfloat2 TexCoord : COLOR;\r};\r\r\r";
				file << "struct VSOutput\r{\r\tfloat4 Position : SV_POSITION; \r\tfloat2 TexCoord : COLOR; \r}; \r\r";
				file << "float4 PSmain(VSOutput input) : SV_TARGET\r{\r\treturn float4(1.0f, 1.0f, 1.0f, 1.0f);\r}";
				
				entryFunc = "PSmain";
				break;
			}

			file.close();

			std::filesystem::path shaderNameStripped = filename;
			auto stem = shaderNameStripped.stem().string();
			m_shaderLib->AddShader(stem, path, entryFunc, shaderType);
		};

		auto ShaderWizard = [&]()
		{
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal("Shader Wizard", 0, ImGuiWindowFlags_NoMove| ImGuiWindowFlags_AlwaysAutoResize))
			{
				static char shaderName[20] = "NewShader.hlsl";
				ImGui::InputText("Script name##inputfield", shaderName, 20, ImGuiInputTextFlags_EnterReturnsTrue);
				
				std::filesystem::path shaderNameStripped = shaderName;
				auto stem = shaderNameStripped.stem().string();

				
				
				if (ImGui::BeginCombo("##shader type", type.c_str()))
				{
					if (ImGui::Selectable("Vertex Shader"))
					{
						shaderType = Graphics::GPUShaderType::Vertex;
						type = "Vertex Shader";
					}
					if (ImGui::Selectable("Pixel Shader"))
					{
						shaderType = Graphics::GPUShaderType::Pixel;
						type = "Pixel Shader";
					}


					ImGui::EndCombo();
				}
				
				if (!m_shaderLib->Exists(stem))
				{
					if (ImGui::Button("Create shader") && shaderType != Graphics::GPUShaderType::None)
					{						
						CreateShaderFile(shaderName, shaderType);
						ImGui::CloseCurrentPopup();
					}
				}
				else
				{
					ImGui::TextColored({ 1, 1 ,0, 1 }, "Shader with the same name already exists!");
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel##shader"))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}
		};

		if (m_shaderLib == nullptr)
		{
			ImGui::TextColored({ 1, 0, 0, 1 }, "Failed to get shader library");
			return;
		}

		
		if (ImGui::BeginTabItem("Shaders"))
		{
			if (ImGui::Button("Create Shader"))
			{
				type = "Choose Shader";
				shaderType = Graphics::GPUShaderType::None;
				ImGui::OpenPopup("Shader Wizard");
			}

			ImGui::SameLine();


			if (ImGui::Button("Hot Reload All Shaders"))
			{
				auto msg = m_shaderLib->HotReloadAll();
				PRX_LOG_INFO("Hot reload info: %s", msg.c_str());
			}

			ShaderWizard();

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
		if (ImGui::BeginTabItem("Audios"))
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
					m_audioLibrary->UpdateSelected(name);
			}

			ImGui::EndTabItem();
		}
	}
	
	void BrowserPanel::DrawMaterialLibrary()
	{
		static bool materialSelected = false;

		if (ImGui::BeginTabItem("Materials"))
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