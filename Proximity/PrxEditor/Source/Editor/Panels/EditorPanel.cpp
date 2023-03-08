#include "editorpch.h"
#include "Editor/Panels/EditorPanel.h"

namespace Proximity::Editor::Panels
{
	EditorPanel::EditorPanel(const std::string& panelName)
		:
		m_panelName(panelName)
	{}

	void EditorPanel::SetPanelName(const std::string& panelName)
	{
		m_panelName = panelName;
	}

	void EditorPanel::DrawPanel()
	{
		BeginPanel();
		Draw();
		EndPanel();
	}

	void EditorPanel::BeginPanel()
	{
		bool open;
		ImGui::Begin(m_panelName.c_str(), &open, ImGuiWindowFlags_None);
	}

	void EditorPanel::EndPanel()
	{
		ImGui::End();
	}
}