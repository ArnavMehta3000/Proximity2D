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
		m_sceneManager  = PRX_RESOLVE(Core::SceneManager);
		m_matLib        = PRX_RESOLVE(Modules::MaterialLibrary);
		m_audioLib      = PRX_RESOLVE(Modules::AudioLibrary);
		m_scriptLibrary = PRX_RESOLVE(Modules::ScriptLibrary);

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
		Core::Entity entity(m_scene->GetSelectedEntity(), m_scene);
		
		if (ImGui::Button("+"))
			ImGui::OpenPopup("Add Component Popup", ImGuiPopupFlags_NoOpenOverExistingPopup);


		if (ImGui::BeginPopup("Add Component Popup"))
		{
			if (ImGui::BeginCombo("##AddComponentDetailsPanel", "Choose Component", ImGuiComboFlags_NoArrowButton))
			{
				// Add audio component
				if (!entity.HasComponent<Core::AudioSourceComponent>() && ImGui::Selectable("Audio Source Component"))
					entity.AddComponent<Core::AudioSourceComponent>();

				if (!entity.HasComponent<Core::RigidBody2DComponent>() && ImGui::Selectable("Rigid Body Component"))
					entity.AddComponent<Core::RigidBody2DComponent>();

				if (!entity.HasComponent<Core::BoxCollider2DComponent>() && ImGui::Selectable("Box Collider Component"))
					entity.AddComponent<Core::BoxCollider2DComponent>();

				if (!entity.HasComponent<Core::LuaScriptComponent>() && ImGui::Selectable("Lua Script Component"))
					entity.AddComponent<Core::LuaScriptComponent>();

				ImGui::EndCombo();
			}
			ImGui::EndPopup();
		}
		

		ImGui::SameLine();

		TryShowNameComponent(entity);
		TryShowTransformComponent(entity);
		TryShowSpriteRendererComponent(entity);
		TryShowLuaScriptComponent(entity);
		TryShowRigidBodyComponent(entity);
		TryShowBoxColliderComponent(entity);
		TryShowAudioSourceComponent(entity);
	}

	void DetailsPanel::TryShowNameComponent(Core::Entity& e)
	{
		DRAW_COMPONENT_DATA(nameComp, "Component Data##NameComponent")

		auto& nameComp = e.GetComponent<Core::NameComponent>();
		ImGui::Text("Entity Name: %s", nameComp.m_EntityName.c_str());
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}

	void DetailsPanel::TryShowTransformComponent(Core::Entity& e)
	{
		if (ImGui::CollapsingHeader("Transform##Details", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DRAW_COMPONENT_DATA(transformComp, "Component Data##TransformComponent")

			auto& transformComp = e.GetComponent<Core::TransformComponent>();

			ImGui::DragFloat3("Position##Transform", &transformComp.m_Position.x, 0.1f);
			ImGui::Spacing();
			ImGui::DragFloat3("Rotation##Transform", &transformComp.m_Rotation.x, 0.1f);
			ImGui::Spacing();
			ImGui::DragFloat3("Scale##Transform", &transformComp.m_Scale.x, 0.1f);
		}

		ImGui::Spacing();
		ImGui::Spacing();
	}

	void DetailsPanel::TryShowRigidBodyComponent(Core::Entity& e)
	{
		if (!e.HasComponent<Core::RigidBody2DComponent>())
			return;
		
		if (ImGui::Button("-"))
		{
			e.RemoveComponent<Core::RigidBody2DComponent>();
			return;
		}
		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Rigid Body 2D", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& rb = e.GetComponent<Core::RigidBody2DComponent>();

			const char* bodyTypeStrings[3] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)rb.m_Type];

			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					if (ImGui::Selectable(bodyTypeStrings[i]))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						rb.m_Type = (Core::RigidBody2DComponent::BodyType)i;
					}
				}

				ImGui::EndCombo();
			}

			ImGui::Checkbox("Fixed Rotation", &rb.m_FixedRotation);
		}

		ImGui::Spacing();
		ImGui::Spacing();
	}

	void DetailsPanel::TryShowBoxColliderComponent(Core::Entity& e)
	{
		if (!e.HasComponent<Core::BoxCollider2DComponent>())
			return;

		if (ImGui::Button("-"))
		{
			e.RemoveComponent<Core::BoxCollider2DComponent>();
			return;
		}
		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Box Collider 2D", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& collider = e.GetComponent<Core::BoxCollider2DComponent>();

			ImGui::DragFloat2("Offset##BoxCollider2D", collider.m_Offset, 0.1f, 0.1f);
			ImGui::DragFloat2("Size##BoxCollider2D", collider.m_Size, 0.1f, 0.1f);
			ImGui::DragFloat("Density##BoxCollider2D", &collider.m_Density, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction##BoxCollider2D", &collider.m_Friction, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution##BoxCollider2D", &collider.m_Restitution, 0.1f, 0.0f, 1.0f);
		}

		ImGui::Spacing();
		ImGui::Spacing();
	}

	void DetailsPanel::TryShowLuaScriptComponent(Core::Entity& e)
	{
		if (!e.HasComponent<Core::LuaScriptComponent>())
			return;

		if (ImGui::Button("-"))
		{
			e.RemoveComponent<Core::LuaScriptComponent>();
			return;
		}
		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Lua Script ##Details", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DRAW_COMPONENT_DATA(audioSrc, "Component Data##AudioSourceComponent")

			auto& script = e.GetComponent<Core::LuaScriptComponent>();

			bool isNull = script.m_Link == nullptr;


			/*if (ImGui::BeginCombo("Source##Choose Script", (isNull) ? "None" : script.m_Link->GetName().c_str(), ImGuiComboFlags_PopupAlignLeft))
			{
				if (ImGui::Selectable("--- CLEAR ---"))
				{
					script.m_Link->UnlinkEntity();
					script.m_Link.reset();
					script.m_Link = nullptr;
				}

				for (auto& pair : m_scriptLibrary->GetMap())
				{
					if (ImGui::Selectable(pair.first.c_str()))
					{
						script.m_Link = pair.second;
						script.m_Link->LinkEntity(e);
					}
				}
				ImGui::EndCombo();
			}

			if (script.m_Link != nullptr)
			{
				if (ImGui::Button("Compile"))
				{
					script.m_Link->Compile();
					script.m_Link->LinkEntity(e);
				}
			}*/
		}

		ImGui::Spacing();
		ImGui::Spacing();
	}

	void DetailsPanel::TryShowSpriteRendererComponent(Core::Entity& e)
	{
		// Check if entity has sprite renderer
		if (!e.HasComponent<Core::SpriteRendererComponent>())
			return;

		auto& srComp = e.GetComponent<Core::SpriteRendererComponent>();


		if (ImGui::CollapsingHeader("Sprite Renderer##Details", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DRAW_COMPONENT_DATA(srComp, "Component Data##SpriteRendererComponent");


			if (ImGui::BeginCombo("Material##SpriteRendererComponent", srComp.m_Material ? srComp.m_Material->GetName().c_str() : "Choose Material", ImGuiComboFlags_PopupAlignLeft))
			{
				if (ImGui::Selectable("-- Clear --"))
				{
					srComp.m_Material.reset();
					srComp.m_Material = nullptr;
				}

				for (auto& pair : m_matLib->GetMap())
				{
					if (ImGui::Selectable(pair.first.c_str()))
						srComp.m_Material = m_matLib->Get(pair.first);
				}

				ImGui::EndCombo();
			}
		}

		ImGui::Spacing();
		ImGui::Spacing();
	}

	void DetailsPanel::TryShowAudioSourceComponent(Core::Entity& e)
	{
		if (!e.HasComponent<Core::AudioSourceComponent>())
			return;

		auto& audioSrc = e.GetComponent<Core::AudioSourceComponent>();

		if (ImGui::Button("-"))
		{
			e.RemoveComponent<Core::AudioSourceComponent>();
			return;
		}
		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Audio Source##Details", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DRAW_COMPONENT_DATA(audioSrc, "Component Data##AudioSourceComponent")

			bool isNull = audioSrc.m_Source == nullptr;
			

			if (ImGui::BeginCombo("Source##Choose Audio", (isNull) ? "None" : audioSrc.m_Source->Name.c_str(), ImGuiComboFlags_PopupAlignLeft))
			{
				if (ImGui::Selectable("--- CLEAR ---"))
				{
					audioSrc.m_Source.reset();
					audioSrc.m_Source = nullptr;
				}

				for (auto& pair : m_audioLib->GetMap())
				{
					if (ImGui::Selectable(pair.first.c_str()))
						audioSrc.m_Source = pair.second;
				}
				ImGui::EndCombo();
			}
		}


		ImGui::Spacing();
		ImGui::Spacing();
	}

}