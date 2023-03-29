#pragma once
#include <PrxEngine.h>

namespace Proximity::Editor::Panels
{
	class EditorPanel
	{
	public:
		EditorPanel(const std::string& panelName);
		virtual ~EditorPanel() {}

		void SetPanelName(const std::string& panelName);
		void DrawPanel();

		static void DrawVec3Control(const std::string& label, DX::XMFLOAT3& values, float resetVal = 0.0f, float columnWidth = 100.0f);
		static void DrawVec4Control(const std::string& label, DX::XMFLOAT4& values, float resetVal = 0.0f, float columnWidth = 100.0f);

	protected:

		virtual void Draw() = 0;

	private:
		void BeginPanel();
		void EndPanel();

		std::string m_panelName;
	};
}