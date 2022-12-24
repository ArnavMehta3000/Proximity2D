#pragma once
#include "Editor/Panels/EditorPanel.h"

namespace Proximity::Editor::Panels
{
	/// <summary>
	/// Editor panel that shows the product of the 2D renderer
	/// </summary>
	class ViewportPanel2D : public EditorPanel
	{
	public:
		ViewportPanel2D();

	private:
		virtual void Draw() override final;

		Graphics::Renderer2D* m_renderer2D;
	};
}