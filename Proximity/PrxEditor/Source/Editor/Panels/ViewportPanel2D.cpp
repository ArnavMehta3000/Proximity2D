#include "editorpch.h"
#include "Editor/Panels/ViewportPanel2D.h"
#include "Graphics/Rendering/Renderer2D.h"

namespace Proximity::Editor::Panels
{
	ViewportPanel2D::ViewportPanel2D()
		:
		EditorPanel("Viewport"),
		m_renderer2D(nullptr)
	{
		using namespace Core::Globals;
		m_renderer2D = m_renderer2D = g_engineServices.ResolveService<Graphics::Renderer2D>();
	}

	void ViewportPanel2D::Draw()
	{
		if (m_renderer2D == nullptr)
		{
			ImGui::TextColored({ 1, 0, 0, 1 }, "Renderer2D not found");
			return;
		}

		ImGui::Image((void*)m_renderer2D->GetFrameBufferSRV().Get(), { 100, 100 });
	}
}