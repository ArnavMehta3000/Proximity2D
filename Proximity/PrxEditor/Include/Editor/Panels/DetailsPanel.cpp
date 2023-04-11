#include "editorpch.h"
#include "Editor/Panels/DetailsPanel.h"
#include "Graphics/Material.h"

#define DEBUG_COMP 0

#if DEBUG_COMP
#define DRAW_COMPONENT_DATA(component, treeName)\
if (ImGui::TreeNode(treeName))\
{\
	ImGui::Text("Component ID: %u", component.m_ComponentID);\
	ImGui::Text("Component View Name: %s", component.m_ViewName.c_str());\
	ImGui::TreePop();\
}
#else
#define DRAW_COMPONENT_DATA(component, treeName)
#endif // DEBUG_COMP




namespace Proximity::Editor::Panels
{
	

	DetailsPanel::DetailsPanel()
		:
		EditorPanel("Details"),
		m_scene(nullptr)
	{
		m_sceneManager = PRX_RESOLVE(Core::SceneManager);
		m_matLib       = PRX_RESOLVE(Modules::MaterialLibrary);
		m_audioLib     = PRX_RESOLVE(Modules::AudioLibrary);

		m_sceneManager->OnSceneLoadOrChanged += PRX_ACTION_FUNC(DetailsPanel::OnWorldSceneChange);
	}

	DetailsPanel::~DetailsPanel()
	{
		m_sceneManager->OnSceneLoadOrChanged -= PRX_ACTION_FUNC(DetailsPanel::OnWorldSceneChange);
	}

	void DetailsPanel::OnWorldSceneChange(Core::Scene* scene)
	{
		m_scene = scene;
	}

	void DetailsPanel::Draw()
	{
		//PRX_ASSERT_MSG(m_scene == nullptr, "ScenePanel::Draw() - scene is nullptr");
		if (m_scene == nullptr)
		{
			ImGui::TextColored({ 1, 1, 0, 1 }, "No scene open");
			return;
		}
		if (m_scene->GetSelectedEntity() == entt::null)
		{
			ImGui::TextColored({ 1, 1, 0, 1 }, "No entity selected");
			return;
		}

		// Cycle through components on the entity
		auto& selected = m_scene->GetSelectedEntity();

		
		if (ImGui::Button("+"))
			ImGui::OpenPopup("Add Component Popup", ImGuiPopupFlags_NoOpenOverExistingPopup);

		if (ImGui::BeginPopup("Add Component Popup"))
		{
			if (ImGui::BeginCombo("##AddComponentDetailsPanel", "Choose Component", ImGuiComboFlags_NoArrowButton))
			{
				if (ImGui::Selectable("Audio Source Component"))
				{
					m_scene->m_sceneRegistry.emplace<Core::AudioSourceComponent>(selected);
				}
				ImGui::EndCombo();
			}
			ImGui::EndPopup();
		}
		

		ImGui::SameLine();

		TryShowNameComponent(selected);
		TryShowTransformComponent(selected);
		TryShowSpriteRendererComponent(selected);
		TryShowAudioSourceComponent(selected);
	}

	void DetailsPanel::TryShowNameComponent(entt::entity& e)
	{
		DRAW_COMPONENT_DATA(nameComp, "Component Data##NameComponent")

		auto& nameComp = m_scene->m_sceneRegistry.get<Core::NameComponent>(e);
		ImGui::Text("Entity Name: %s", nameComp.m_EntityName.c_str());
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}

	void DetailsPanel::TryShowTransformComponent(entt::entity& e)
	{
		if (ImGui::CollapsingHeader("Transform##Details", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DRAW_COMPONENT_DATA(transformComp, "Component Data##TransformComponent")

			auto& transformComp = m_scene->m_sceneRegistry.get<Core::TransformComponent>(e);
			/*DrawVec3Control("Position", transformComp.m_Position);
			ImGui::Spacing();
			DrawVec3Control("Rotation", transformComp.m_Rotation);
			ImGui::Spacing();
			DrawVec3Control("Scale", transformComp.m_Scale);*/

			ImGui::DragFloat3("Position##Transform", &transformComp.m_Position.x, 0.1f);
			ImGui::Spacing();
			ImGui::DragFloat3("Rotation##Transform", &transformComp.m_Rotation.x, 0.1f);
			ImGui::Spacing();
			ImGui::DragFloat3("Scale##Transform", &transformComp.m_Scale.x, 0.1f);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}


	void DetailsPanel::TryShowSpriteRendererComponent(entt::entity& e)
	{
		// Check if entity has sprite renderer
		if (!m_scene->m_sceneRegistry.all_of<Core::SpriteRendererComponent>(e))
			return;

		auto& srComp = m_scene->m_sceneRegistry.get<Core::SpriteRendererComponent>(e);


		if (ImGui::CollapsingHeader("Sprite Renderer##Details", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DRAW_COMPONENT_DATA(srComp, "Component Data##SpriteRendererComponent");


			if (ImGui::BeginCombo("Material##SpriteRendererComponent", srComp.Material ? srComp.Material->GetName().c_str() : "Choose Material", ImGuiComboFlags_PopupAlignLeft))
			{
				if (ImGui::Selectable("-- Clear --"))
				{
					srComp.Material.reset();
					srComp.Material = nullptr;
				}

				for (auto& pair : m_matLib->GetMap())
				{
					if (ImGui::Selectable(pair.first.c_str()))
						srComp.Material = m_matLib->Get(pair.first);
				}

				ImGui::EndCombo();
			}

			if (srComp.Material)
			{
				ImGui::Spacing();
				if (ImGui::Selectable("See Asset Info panel for material properties", false, ImGuiSelectableFlags_AllowDoubleClick))
				{
					// Show material asset info if double clicked
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						m_matLib->OnMaterialSelected(srComp.Material->GetName());
						ImGui::SetWindowFocus("Asset Info");
					}
				}

			}
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}

	void DetailsPanel::TryShowAudioSourceComponent(entt::entity& e)
	{
		if (!m_scene->m_sceneRegistry.any_of<Core::AudioSourceComponent>(e))
			return;

		auto& audioSrc = m_scene->m_sceneRegistry.get<Core::AudioSourceComponent>(e);

		if (ImGui::Button("-"))
		{
			m_scene->m_sceneRegistry.remove<Core::AudioSourceComponent>(e);
			return;
		}
		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Audio Source##Details", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DRAW_COMPONENT_DATA(audioSrc, "Component Data##AudioSourceComponent")

			bool isNull = audioSrc.Source == nullptr;
			

			if (ImGui::BeginCombo("##Choose Audio", (isNull) ? "None" : audioSrc.Source->Name.c_str(), ImGuiComboFlags_PopupAlignLeft))
			{
				if (ImGui::Selectable("--- CLEAR ---"))
				{
					audioSrc.Source.reset();
					audioSrc.Source = nullptr;
				}

				for (auto& pair : m_audioLib->GetMap())
				{
					if (ImGui::Selectable(pair.first.c_str()))
						audioSrc.Source = pair.second;
				}
				ImGui::EndCombo();
			}
		}


		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}

}