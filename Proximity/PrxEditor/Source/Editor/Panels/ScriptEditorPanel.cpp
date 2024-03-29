#include "editorpch.h"
#include "Editor/Panels/ScriptEditorPanel.h"
#include "optick/include/optick.h"

namespace Proximity::Editor::Panels
{
	ScriptEditorPanel::ScriptEditorPanel()
		:
		EditorPanel("Script Editor")
	{
		m_editor    = std::make_unique<TextEditor>();
		m_scriptLib = PRX_RESOLVE(Modules::ScriptLibrary);
		//m_scriptLib->OnScriptSelected += PRX_ACTION_FUNC(ScriptEditorPanel::ScriptSelected);
		SetupEditor();
	}

	void ScriptEditorPanel::SetupEditor()
	{
		TextEditor::LanguageDefinition lang = TextEditor::LanguageDefinition::Lua();

		static const std::pair<const char*, const char*> ppNameVals[] =
		{
			{"PRX"               , "Global lua table key that contains all engine functions"},
			{"_Entity"           , "Global lua table key that contains all entity functions"},
			{"Vector3"           , "Vector3 class with X, Y, Z components"},
			{"Transform"         , "Class with position, rotation & scale components"},
			{"CollisionManifold" , "Contains collision data"},
			{"RaycastManifold"   , "Contains raycast data"},
		};

		for (int i = 0; i < ARRAYSIZE(ppNameVals); i++)
		{
			TextEditor::Identifier id;
			id.mDeclaration = ppNameVals[i].second;
			lang.mPreprocIdentifiers.try_emplace(std::string(ppNameVals[i].first), id);
		}
		
		static const std::pair<const char*, const char*> identifierDecls[] =
		{
			{"OnStart"          , "Function called when play mode is entered"},
			{"OnUpdate"         , "Function called every frame"},
			{"OnKeyDown"        , "Function called when any (supported) keybaord key is pressed"},
			{"OnKeyUp"          , "Function called when any (supported) keybaord key is released"},
			
			{"OnCollisionStart" , "Function called when object starts colliding with another object"},
			{"OnCollisionEnd"   , "Function called when object stopss colliding with another object"},
			
			{"Log"              , "Log a message to the editor console"},
			{"LogVec3"          , "Log a Vector3 to the editor console"},
			{"LogTransform"     , "Log a Transform to the editor console"},
			{"CheckKey"         , "Checks if the input key is equal to comaprison key"},
			
			{"GetTransform"     , "Get '_Entity' transform"},
			{"SetTransform"     , "Set '_Entity' transform"},
			{"GetPosition"      , "Get '_Entity' position"},
			{"SetRotation"      , "Set '_Entity' rotation"},
			{"GetRotation"      , "Get '_Entity' rotation"},
			{"SetPosition"      , "Set '_Entity' position"},
			{"GetScale"         , "Get '_Entity' scale"},
			{"SetScale"         , "Set '_Entity' scale"},
			
			{"DoRaycast"        , "Do Physics2D raycast through _Entity"},
			{"ApplyForce"       , "Apply force through _Entity"},
			
			{"PlayAudio"        , "Play audio clip through _Entity"},
			{"PlayAudioOneShot" , "Play audio one shot through _Entity"},
		};

		for (int i = 0; i < ARRAYSIZE(identifierDecls); i++)
		{
			TextEditor::Identifier id;
			id.mDeclaration = std::string(identifierDecls[i].second);
			lang.mIdentifiers.try_emplace(std::string(identifierDecls[i].first), id);
		}

		m_editor->SetLanguageDefinition(lang);
		m_editor->SetShowWhitespaces(false);
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

		

		if (!m_activeScriptPath.empty())
		{
			if (ImGui::Button("Save"))
				SaveFile();
			
			// Removed compiling button from here

			ImGui::SameLine();
		}


		if (ImGui::BeginCombo("File", filename.c_str(), ImGuiComboFlags_PopupAlignLeft))
		{
			for (const auto& pair : m_scriptLib->GetMap())
			{
				if (ImGui::Selectable(pair.first.c_str()))
				{
					// If switching and file is unsaved -> save it
					if (!m_activeScriptPath.empty()&& m_editor->CanUndo())
						SaveFile();

					ScriptSelected(pair.second->GetFilePath());
				}
			}

			ImGui::EndCombo();
		}
		ImGui::SameLine();

		if (ImGui::BeginCombo("Theme", "Choose theme", ImGuiComboFlags_PopupAlignLeft))
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
		OPTICK_EVENT("ScriptEditorPanel::Draw")
		DrawMenuBar();

		m_editor->Render("Lua File");
	}
	
	void ScriptEditorPanel::SaveFile()
	{
		if (m_activeScriptPath.empty())
			return;

		std::ofstream file(m_activeScriptPath);
		auto text = m_editor->GetText();

		// Remove any trailing newline characters
		while (!text.empty() && text.back() == '\n')
			text.pop_back();

		file << text;
		file.close();

		// Reload script
		ScriptSelected(m_activeScriptPath);
	}
}