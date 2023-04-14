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

		void TryShowNameComponent(Core::Entity& e);
		void TryShowTransformComponent(Core::Entity& e);
		void TryShowSpriteRendererComponent(Core::Entity& e);
		void TryShowAudioSourceComponent(Core::Entity& e);
		void TryShowRigidBodyComponent(Core::Entity& e);
		void TryShowBoxColliderComponent(Core::Entity& e);

		Core::Scene*              m_scene;
		Core::SceneManager*       m_sceneManager;

		Modules::MaterialLibrary* m_matLib;
		Modules::AudioLibrary*    m_audioLib;
	};
}