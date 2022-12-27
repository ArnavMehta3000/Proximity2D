#include "editorpch.h"
#include "Editor/Panels/ScenePanel.h"

namespace Proximity::Editor::Panels
{
	ScenePanel::ScenePanel()
		:
		EditorPanel("Scene"),
		m_scene(nullptr)
	{
		auto sm = Core::Globals::g_engineServices.ResolveService<Core::SceneManager>();
		sm->OnSceneLoadOrChanged += PRX_ACTION_FUNC(ScenePanel::OnWorldSceneChange);
	}
	
	ScenePanel::~ScenePanel()
	{
		auto sm = Core::Globals::g_engineServices.ResolveService<Core::SceneManager>();
		sm->OnSceneLoadOrChanged -= PRX_ACTION_FUNC(ScenePanel::OnWorldSceneChange);
	}

	void ScenePanel::OnWorldSceneChange(const Core::Scene* scene)
	{
		m_scene = scene;
	}

	void ScenePanel::Draw()
	{
		// Force get scene (in case the editor panel is initialized after the scene creation
		if (m_scene == nullptr)
		{
			ImGui::TextColored({ 1, 0, 0, 1 }, "No active scene");
			return;
		}
		

		//// Get all the entities
		//auto view = m_scene->GetEntityRegistery().view<Core::NameComponent>();
		//auto treeNodeFlags = ImGuiTreeNodeFlags_SpanFullWidth;
		//bool alignX = true;
		//int nodeClicked = -1;

		//for (int i = 0; i < view.size(); i++)
		//{
		//	auto entity = view[i];
		//	auto& nameComp = m_scene->GetEntityRegistery().get<Core::NameComponent>(entity);

		//	bool nodeOpen = ImGui::TreeNodeEx(std::to_string(nameComp.m_ComponentID).c_str());
		//	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		//		nodeClicked = i;

		//	// TODO: Handle children here
		//	if (ImGui::BeginDragDropSource())
		//	{
		//		ImGui::SetDragDropPayload("_TREENODE", NULL, 0);;
		//		ImGui::EndDragDropSource();
		//	}
		//	if (nodeOpen)
		//		ImGui::TreePop();
		//}
	}
}