#include "editorpch.h"
#include "Editor/Panels/EditorConsolePanel.h"

namespace Proximity::Editor::Panels
{
	EditorConsolePanel::EditorConsolePanel()
		:
		EditorPanel("Editor Console")
	{
	}
	
	void EditorConsolePanel::Draw()
	{
		namespace G = Proximity::Core::Globals;
		auto size = G::g_debugBuffer->GetStreamSize();
		ImGui::AlignTextToFramePadding();
		if (ImGui::Button("Add"))
			G::g_debugBuffer->AddToStream(std::to_string(Math::Random::Value()));

		for (Math::U64 i = size - 1; i >= 0; i--)
		{
			if (i < 0 || i >= size)
				break;

			ImGui::Text("%s", G::g_debugBuffer->GetStreamBuffer()[i].str().c_str());
		}

		if (ImGui::Button("Clear"))
			G::g_debugBuffer->ClearAll();
	}
}