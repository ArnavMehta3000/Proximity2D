#include "editorpch.h"
#include "Editor/Panels/EditorPanel.h"

namespace Proximity::Editor
{
	EditorPanel::EditorPanel(const std::string& panelName)
		:
		m_panelName(panelName)
	{}

	void EditorPanel::SetName(const std::string& panelName)
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
		ImGui::Begin(m_panelName.c_str());
	}
	void EditorPanel::EndPanel()
	{
		ImGui::End();
	}
}