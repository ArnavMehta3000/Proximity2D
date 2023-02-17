#pragma once
#include "Editor/Panels/EditorPanel.h"

namespace Proximity::Editor::Panels
{
	class EditorConsolePanel : public EditorPanel
	{
	public:
		EditorConsolePanel();

	private:
		virtual void Draw() override;
		void DrawEngineConsole();
		void DrawEditorConsole();
	};
}