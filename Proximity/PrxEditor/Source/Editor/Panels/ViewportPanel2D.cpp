#include "editorpch.h"
#include "Editor/Panels/ViewportPanel2D.h"
#include "Graphics/Rendering/Renderer2D.h"

namespace Proximity::Editor::Panels
{
	ViewportPanel2D::ViewportPanel2D()
		:
		EditorPanel("Viewport"),
		m_renderer2D(nullptr),
		m_viewportSize(Math::Vec3::Zero)
	{
		using namespace Core::Globals;
		m_renderer2D = m_renderer2D = g_engineServices.ResolveService<Graphics::Renderer2D>();
	}


	// Resize ref: https://math.stackexchange.com/questions/180804/how-to-get-the-aspect-ratio-of-an-image
	// https://stackoverflow.com/questions/1222340/aspect-ratios-how-to-go-about-them-d3d-viewport-setup
	void ViewportPanel2D::Draw()
	{
		
		if (m_renderer2D == nullptr)
		{
			ImGui::TextColored({ 1, 0, 0, 1 }, "Renderer2D not found");
			return;
		}


		auto vpSize = ImGui::GetContentRegionAvail();
		if (vpSize.x < 10.0f || vpSize.y < 10.0f)
			return;		

		if (m_viewportSize.x != vpSize.x || m_viewportSize.y != vpSize.y)
		{
			m_viewportSize.x = vpSize.x;
			m_viewportSize.y = vpSize.y;
			m_viewportSize.z = 0.0f;

			m_renderer2D->Resize((Math::U32)vpSize.x, (Math::U32)vpSize.y);

			//PRX_LOG_INFO("Editor Viewport resize: {%f, %f}", vpSize.x, vpSize.y);
			return;
		}
		ImGui::Image((void*)m_renderer2D->GetEditorFrameBuffer().Texture.SRV.Get(), vpSize);
		
	}
}