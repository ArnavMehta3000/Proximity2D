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
		ViewportPanel2D(Core::OrthographicCamera* editorCam);

	private:
		virtual void Draw() override final;

		void CheckMouse();
		
	private:
		Core::OrthographicCamera* m_editorCamera;
		ImVec2                    m_relativeMousePos;
		Math::Vector3             m_viewportSize;
		Graphics::Renderer2D*     m_renderer2D;
	};
}