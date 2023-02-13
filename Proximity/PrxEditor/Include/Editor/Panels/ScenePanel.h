#pragma once
#include "Editor/Panels/EditorPanel.h"

namespace Proximity::Editor::Panels
{
	class ScenePanel : public EditorPanel
	{
	public:
		ScenePanel();
		~ScenePanel();

	private:
		void OnWorldSceneChange(Core::Scene* scene);

		virtual void Draw() override final;
		void DrawEntityCreationWizard();

		Core::Scene* m_scene;
		Core::SceneManager* m_sceneManager;
	};
}