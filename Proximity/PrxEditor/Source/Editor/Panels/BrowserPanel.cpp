#include "editorpch.h"
#include "Editor/Panels/BrowserPanel.h"

namespace Proximity::Editor::Panels
{
	BrowswerPanel::BrowswerPanel()
		:
		EditorPanel("Content Browswer")
	{

	}
	void BrowswerPanel::Draw()
	{
		ImGui::Text("Refer to content browser here");
	}
}