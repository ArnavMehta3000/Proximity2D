#include "editorpch.h"
#include "Editor/Panels/ViewportPanel2D.h"
#include "Graphics/Rendering/Renderer2D.h"

namespace Proximity::Editor::Panels
{
	ViewportPanel2D::ViewportPanel2D(Core::OrthographicCamera* editorCam)
		:
		EditorPanel("Viewport"),
		m_renderer2D(nullptr),
		m_viewportSize(Math::Vec3::Zero),
		m_editorCamera(editorCam)
	{
		using namespace Core::Globals;
		m_renderer2D = m_renderer2D = PRX_RESOLVE(Graphics::Renderer2D);
	}


	// Resize ref: https://math.stackexchange.com/questions/180804/how-to-get-the-aspect-ratio-of-an-image
	// https://stackoverflow.com/questions/1222340/aspect-ratios-how-to-go-about-them-d3d-viewport-setup
	void ViewportPanel2D::Draw()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		{
			if (ImGui::IsMouseDragging(2))
			{
				Vector3 pos = m_editorCamera->Position();
				auto delta  = ImGui::GetMouseDragDelta(2);
				
				pos.x -= delta.x * 0.1f;
				pos.y += delta.y * 0.1f;
				
				ImGui::ResetMouseDragDelta(2);

				m_editorCamera->Position(Vector3(pos.x, pos.y, pos.z));
			}


			auto scale = m_editorCamera->GetOrthoScale();
			scale += ImGui::GetIO().MouseWheel * 0.1f;
			m_editorCamera->OrthoScale(scale);
		}

		
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

			//m_renderer2D->Resize((Math::U32)vpSize.x, (Math::U32)vpSize.y);
			Core::Globals::g_vpResizeEvent((Math::U32)vpSize.x, (Math::U32)vpSize.y);
			//return;
		}

		ImGui::Image((void*)m_renderer2D->GetEditorFrameBuffer().Texture.SRV.Get(), vpSize);

		ImGui::PopStyleVar();
	}

	void ViewportPanel2D::CheckMouse()
	{
		auto& io   = ImGui::GetIO();
		ImVec2 winPos      = ImGui::GetWindowPos();
		ImVec2 mousePos    = io.MousePos;
		m_relativeMousePos = { mousePos.x - winPos.x, mousePos.y - winPos.y };
		
		if (io.MouseClicked[0])
			PRX_LOG_INFO("Mouse Left (%f, %f)", m_relativeMousePos.x, m_relativeMousePos.y);
	}
}