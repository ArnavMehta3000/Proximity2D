#include "editorpch.h"
#include "Editor/Panels/ScenePanel.h"

namespace Proximity::Editor::Panels
{
	static char s_entityName[25] = "Entity";

	ScenePanel::ScenePanel(Core::OrthographicCamera* editorCam)
		:
		EditorPanel("Scene"),
		m_scene(nullptr),
		m_editorCamera(editorCam)
	{
		m_sceneManager = PRX_RESOLVE(Core::SceneManager);
		m_sceneManager->OnSceneLoadOrChanged += PRX_ACTION_FUNC(ScenePanel::OnWorldSceneChange);
	}
	
	ScenePanel::~ScenePanel()
	{
		m_sceneManager->OnSceneLoadOrChanged -= PRX_ACTION_FUNC(ScenePanel::OnWorldSceneChange);
	}

	void ScenePanel::OnWorldSceneChange(Core::Scene* scene)
	{
		m_scene = scene;
	}

	void ScenePanel::Draw()
	{
		{
			ImGui::TextDisabled("Editor Camera");
			ImGui::SameLine();
			Math::Vector3 pos = m_editorCamera->Position();
			if (ImGui::DragFloat3("##EditorCamera", &pos.x, 0.1f))
				m_editorCamera->Position(Math::Vector3(pos.x, pos.y, pos.z));
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// Force get scene (in case the editor panel is initialized after the scene creation
		if (m_scene == nullptr)
		{
			bool open = false;
			ImGui::Selectable("No active scene", open, ImGuiSelectableFlags_Disabled);
			return;
		}
		bool open = true;

		if (ImGui::Button("Create Entity"))
			ImGui::OpenPopup("Create Entity Wizard");

		ImGui::SameLine();

		ImGui::Selectable(m_scene->GetName().c_str(), open, ImGuiSelectableFlags_Disabled);
		DrawEntityCreationWizard();


		// Get all the entities
		auto view = m_scene->m_sceneRegistry.view<Core::NameComponent>();
		for (int i = 0; i < view.size(); i++)
		{
			auto entity = view[i];
			auto& nameComp = m_scene->m_sceneRegistry.get<Core::NameComponent>(entity);

			static bool selected = false;
			if (ImGui::Selectable(nameComp.m_EntityName.c_str(), selected))
			{
				m_scene->SetSelectedEntity(entity);
			}
		}
	}

	void ScenePanel::DrawEntityCreationWizard()
	{
		if (ImGui::BeginPopup("Create Entity Wizard"))
		{
			bool rename = ImGui::InputText("Entity name##inputfield", s_entityName, 25, ImGuiInputTextFlags_EnterReturnsTrue);
			
			if (ImGui::Button("Create##Entity") || rename)
				m_scene->CreateEntity(s_entityName);

			ImGui::EndPopup();
		}
	}
}