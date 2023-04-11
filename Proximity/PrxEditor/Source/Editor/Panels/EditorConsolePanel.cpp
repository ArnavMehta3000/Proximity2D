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
		auto size = Proximity::Core::Globals::g_engineDebugBuffer->GetStreamSize();
		if (ImGui::BeginTabItem("Engine Console"))
		{
			
			ImGui::BeginChild("##engineLog", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
			
			for (Math::U64 i = 0; i < size; i++)
			{
				auto str = Proximity::Core::Globals::g_engineDebugBuffer->GetStreamBuffer()[i].str();

				if (str.empty())
					continue;

				ImGui::Text("%s%s", "[INTERNAL]", str.c_str());
			}

			ImGui::EndChild();
			ImGui::EndTabItem();
		}
	}
	void EditorConsolePanel::DrawEditorConsole()
	{
		if (ImGui::BeginTabItem("Editor Console"))
		{
			auto size = Proximity::Core::Globals::g_editorDebugBuffer->GetStreamSize();

			if (ImGui::SmallButton("Clear Logs"))
				Proximity::Core::Globals::g_editorDebugBuffer->ClearAll();


			ImGui::BeginChild("##editorLog", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
			
			for (Math::U64 i = 0; i < size; i++)
			{
				auto str = Proximity::Core::Globals::g_editorDebugBuffer->GetStreamBuffer()[i].str();

				if (str.empty())
					continue;

				ImGui::Text("%s", str.c_str());
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
	}
}