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
		if (ImGui::BeginTabBar("Content Browser Tabs"))
		{
			DrawTextureLibrary();
			DrawShaderLibrary();
			DrawAudioibrary();

			ImGui::EndTabBar();
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