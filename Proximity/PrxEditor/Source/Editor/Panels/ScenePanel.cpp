#include "editorpch.h"
#include "Editor/Panels/ScenePanel.h"

namespace Proximity::Editor
{
	ScenePanel::ScenePanel()
		:
		EditorPanel("Scene")
	{}

	void ScenePanel::Draw()
	{
		ImGui::Text("This is the scene panel");
	}
}