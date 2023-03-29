#include "editorpch.h"
#include "Editor/Panels/EditorPanel.h"
#include "imgui/imgui_internal.h"

namespace Proximity::Editor::Panels
{
	EditorPanel::EditorPanel(const std::string& panelName)
		:
		m_panelName(panelName)
	{}

	void EditorPanel::SetPanelName(const std::string& panelName)
	{
		m_panelName = panelName;
	}

	void EditorPanel::DrawPanel()
	{
		BeginPanel();
		Draw();
		EndPanel();
	}

	void EditorPanel::DrawVec3Control(const std::string& label, DX::XMFLOAT3& values, float resetVal, float columnWidth)
	{
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();
		
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
		
		// X
		{
			ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.25f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			if (ImGui::Button("X", buttonSize))
				values.x = resetVal;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##X", &values.x, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}
		// Y
		{
			ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.7f, 0.4f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.3f, 0.7f, 0.4f, 1.0f });
			if (ImGui::Button("Y", buttonSize))
				values.y = resetVal;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##", &values.y, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}
		// Z
		{
			ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			if (ImGui::Button("Z", buttonSize))
				values.z = resetVal;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Z", &values.z, 0.1f);
			ImGui::PopItemWidth();
		}

		ImGui::PopStyleVar();
		ImGui::Columns(1);
	}

	void EditorPanel::DrawVec4Control(const std::string& label, DX::XMFLOAT4& values, float resetVal, float columnWidth)
	{
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		// X
		{
			ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.25f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			if (ImGui::Button("X", buttonSize))
				values.x = resetVal;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##X", &values.x, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}
		// Y
		{
			ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.7f, 0.4f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.3f, 0.7f, 0.4f, 1.0f });
			if (ImGui::Button("Y", buttonSize))
				values.y = resetVal;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##", &values.y, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}
		// Z
		{
			ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			if (ImGui::Button("Z", buttonSize))
				values.z = resetVal;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Z", &values.z, 0.1f);
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}
		// W
		{
			ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.0f, 0.3f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.1f, 0.4f, 0.4f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.1f, 0.4f, 0.4f, 1.0f });
			if (ImGui::Button("W", buttonSize))
				values.w = resetVal;
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##W", &values.w, 0.1f);
			ImGui::PopItemWidth();
		}

		ImGui::PopStyleVar();
		ImGui::Columns(1);
	}

	void EditorPanel::BeginPanel()
	{
		bool open;
		ImGui::Begin(m_panelName.c_str(), &open, ImGuiWindowFlags_None);
	}

	void EditorPanel::EndPanel()
	{
		ImGui::End();
	}
}