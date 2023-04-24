#pragma once
#include "Editor/Panels/EditorPanel.h"
#include "Editor/TextEditor.h"


namespace Proximity::Editor::Panels
{
	class ScriptEditorPanel : public EditorPanel
	{
	public:
		ScriptEditorPanel();

		void SetupEditor();

	private:
		void ScriptSelected(const std::string_view& scriptPath);
		void DrawMenuBar();
		virtual void Draw() override final;

		void SaveFile();

	private:
		std::string                 m_activeScriptPath;
		std::unique_ptr<TextEditor> m_editor;
		Modules::ScriptLibrary*     m_scriptLib;
	};
}