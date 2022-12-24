#pragma once
#include <PrxEngine.h>

namespace Proximity::Editor::Panels
{
	class EditorPanel
	{
	public:
		EditorPanel(const std::string& panelName);
		
		void SetName(const std::string& panelName);
		void DrawPanel();

	protected:

		virtual void Draw() = 0;

	private:
		void BeginPanel();
		void EndPanel();

		std::string m_panelName;
	};
}