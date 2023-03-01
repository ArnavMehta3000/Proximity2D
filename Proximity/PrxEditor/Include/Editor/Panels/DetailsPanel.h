#pragma once
#include "Editor/Panels/EditorPanel.h"
#include "Editor/Panels/ScenePanel.h"

namespace Proximity::Editor::Panels
{
	class DetailsPanel : public EditorPanel
	{
	public:
		DetailsPanel();
		~DetailsPanel();

	private:
		virtual void Draw() override final;

		void OnWorldSceneChange(Core::Scene* scene);

		void TryShowNameComponent(entt::entity& e);
		void TryShowTransformComponent(entt::entity& e);
		void TryShowSpriteRendererComponent(entt::entity& e);

		void DrawShaderVarByType(const Graphics::GPUShaderVariable& var);


		Core::Scene*        m_scene;
		Core::SceneManager* m_sceneManager;
	};
}