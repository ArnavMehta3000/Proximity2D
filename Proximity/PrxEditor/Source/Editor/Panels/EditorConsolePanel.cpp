#include "editorpch.h"
#include "Editor/Panels/EditorConsolePanel.h"
namespace Proximity::Editor::Panels
{
	EditorConsolePanel::EditorConsolePanel()
		:
		EditorPanel("Consoles")
	{
	}
	
	void EditorConsolePanel::Draw()
	{
		if (ImGui::BeginTabBar("Consoles", ImGuiTabBarFlags_Reorderable))
		{
			DrawEngineConsole();
			DrawEditorConsole();

			ImGui::EndTabBar();
		}
	}
	void EditorConsolePanel::DrawEngineConsole()
	{
		if (ImGui::BeginTabItem("Engine Console"))
		{
			ImGui::Text(Utils::TextBuffer::GetStaticStream().str().c_str());
			
			ImGui::EndTabItem();
		}
	}
	void EditorConsolePanel::DrawEditorConsole()
	{
		if (ImGui::BeginTabItem("Editor Console"))
		{
			auto size = Proximity::Core::Globals::g_debugBuffer->GetStreamSize();
			if (ImGui::Button("Clear"))
				Proximity::Core::Globals::g_debugBuffer->ClearAll();

			if (ImGui::Button("Add"))
				Proximity::Core::Globals::g_debugBuffer->AddToStream("This is a test");

			for (Math::U64 i = 0; i < size; i++)
			{
				if (Proximity::Core::Globals::g_debugBuffer->GetStreamBuffer()[i].str().empty())
					continue;

				ImGui::Text("ID: %u     %s", i, Proximity::Core::Globals::g_debugBuffer->GetStreamBuffer()[i].str().c_str());
			}
			ImGui::EndTabItem();
		}
	}
}