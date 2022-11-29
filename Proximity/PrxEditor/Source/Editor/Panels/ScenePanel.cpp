#include "editorpch.h"
#include "Editor/Panels/ScenePanel.h"

namespace Proximity::Editor
{
	ScenePanel::ScenePanel()
		:
		EditorPanel("Scene"),
		m_scene(nullptr)
	{
		WORLD->OnSceneChange += PRX_ACTION_FUNC(ScenePanel::OnWorldSceneChange);
	}

	void ScenePanel::OnWorldSceneChange(Core::Scene* scene)
	{
		m_scene = scene;
	}

	void ScenePanel::Draw()
	{
		if (m_scene == nullptr)
			return;

		// Get all the entities
		auto view = m_scene->GetEntityRegistery().view<Core::NameComponent>();
		for (auto& entity : view)
		{
			auto& t = m_scene->GetEntityRegistery().get<Core::NameComponent>(entity);
			ImGui::Text(t.m_EntityName.c_str());
		}

		ImGui::Text("This is the scene panel");
	}
}