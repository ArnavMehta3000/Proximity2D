#include "editorpch.h"
#include "Editor/Panels/ViewportPanel2D.h"
#include "Graphics/Rendering/Renderer2D.h"

namespace Proximity::Editor::Panels
{
	ViewportPanel2D::ViewportPanel2D()
		:
		EditorPanel("Viewport"),
		m_renderer2D(nullptr),
		m_viewPortSize(Math::Vec3::Zero)
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


		auto vpSize = ImGui::GetContentRegionAvail();
		if (m_viewPortSize.x != vpSize.x || m_viewPortSize.y != vpSize.y)
		{
			m_viewPortSize.x = vpSize.x;
			m_viewPortSize.y = vpSize.y;
			m_viewPortSize.z = 0.0f;
			m_renderer2D->Resize((Math::U32)m_viewPortSize.x, (Math::U32)m_viewPortSize.y);
			//PRX_LOG_INFO("Editor Viewport resize: {%f, %f}", m_viewPortSize.x, m_viewPortSize.y);
			return;
		}
		ImGui::Image((void*)m_renderer2D->GetFrameBuffer().Texture.SRV.Get(), vpSize);
	}
}