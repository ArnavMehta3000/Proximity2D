#include "editorpch.h"
#include "Editor/Panels/DetailsPanel.h"

namespace Proximity::Editor::Panels
{
	DetailsPanel::DetailsPanel()
		:
		EditorPanel("Details"),
		m_scene(nullptr)
	{
		m_sceneManager = Core::Globals::g_engineServices.ResolveService<Core::SceneManager>();
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

		TryShowNameComponent(selected);
		TryShowTransformComponent(selected);
		TryShowSpriteRendererComponent(selected);
	}

	void DetailsPanel::TryShowNameComponent(entt::entity& e)
	{
		auto& nameComp = m_scene->m_sceneRegistry.get<Core::NameComponent>(e);
		if (ImGui::CollapsingHeader("Data##details"))
		{
			ImGui::Text("Entity Name: %s", nameComp.m_EntityName.c_str());
		}
	}
	void DetailsPanel::TryShowTransformComponent(entt::entity& e)
	{
		auto& transformComp = m_scene->m_sceneRegistry.get<Core::TransformComponent>(e);
		if (ImGui::CollapsingHeader("Transform##Details"))
		{
			ImGui::DragFloat3("Position##Transform", &transformComp.m_Position.x, 0.01f);
			ImGui::DragFloat("Rotation##Transform", &transformComp.m_Rotation, 0.01f);
			ImGui::DragFloat3("Scale##Transform", &transformComp.m_Scale.x, 0.01f);
		}
	}
	void DetailsPanel::TryShowSpriteRendererComponent(entt::entity& e)
	{
		// Check if entity has sprite renderer
		if (!m_scene->m_sceneRegistry.all_of<Core::SpriteRendererComponent>(e))
			return;
		
		auto& srComp = m_scene->m_sceneRegistry.get<Core::SpriteRendererComponent>(e);
		if (ImGui::CollapsingHeader("Sprite Renderer##Setails"))
		{
			ImGui::ColorEdit4("Tint##SRComponent", &srComp.Tint.x, ImGuiColorEditFlags_InputRGB);
		}
	}
}