#include "editorpch.h"
#include "Editor/Panels/ScriptEditorPanel.h"

namespace Proximity::Editor::Panels
{
	ScriptEditorPanel::ScriptEditorPanel()
		:
		EditorPanel("Script Editor")
	{
		m_editor    = std::make_unique<TextEditor>();
		m_scriptLib = PRX_RESOLVE(Modules::ScriptLibrary);
		//m_scriptLib->OnScriptSelected += PRX_ACTION_FUNC(ScriptEditorPanel::ScriptSelected);
		Setup();
	}

	void ScriptEditorPanel::Setup()
	{
		m_editor->SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
	}

	void ScriptEditorPanel::ScriptSelected(const std::string_view& scriptPath)
	{
		// Save text here
		std::ifstream t(scriptPath.data());
		if (t.good())
		{
			m_activeScriptPath = scriptPath;
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			m_editor->SetText(str);

			t.close();
		}

	}

	void ScriptEditorPanel::DrawMenuBar()
	{
		auto filename = Utils::DirectoryManager::GetFileNameFromDir(m_activeScriptPath);
		if (m_activeScriptPath.empty())
		{
			ImGui::BeginDisabled();
			if (ImGui::Button("Save"))
				SaveFile();
			ImGui::EndDisabled();
		}
		else
		{
			if (ImGui::Button("Save"))
				SaveFile();
		}
		ImGui::SameLine();

		if (ImGui::BeginCombo("File", filename.c_str()))
		{
			for (const auto& path : m_scriptLib->GetScriptsPathList())
			{
				auto file = Utils::DirectoryManager::GetFileNameFromDir(path);
				if (ImGui::Selectable(file.c_str()))
					ScriptSelected(path);
			}

			ImGui::EndCombo();
		}
		ImGui::SameLine();

		if (ImGui::BeginCombo("##Theme", "Choose theme"))
		{
			if (ImGui::Selectable("Dark palette"))
				m_editor->SetPalette(TextEditor::GetDarkPalette());
			if (ImGui::Selectable("Light palette"))
				m_editor->SetPalette(TextEditor::GetLightPalette());
			if (ImGui::Selectable("Retro blue palette"))
				m_editor->SetPalette(TextEditor::GetRetroBluePalette());

			ImGui::EndCombo();
		}

		auto cpos = m_editor->GetCursorPosition();
		ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, m_editor->GetTotalLines(),
			m_editor->IsOverwrite() ? "Ovr" : "Ins",
			m_editor->CanUndo() ? "*" : " ",
			m_editor->GetLanguageDefinition().mName.c_str(), filename.c_str());

	}

	void ScriptEditorPanel::Draw()
	{
		DrawMenuBar();

		m_editor->Render("Lua File");
	}
	
	void ScriptEditorPanel::SaveFile()
	{
		if (m_activeScriptPath.empty())
			return;

		std::ofstream file(m_activeScriptPath);
		file << m_editor->GetText();
		file.close();
	}
}