#pragma once
#include "Editor/Panels/EditorPanel.h"

namespace Proximity::Editor::Panels
{
	class ScenePanel : public EditorPanel
	{
	public:
		ScenePanel(Core::OrthographicCamera* editorCam);
		~ScenePanel();

	private:
		void OnWorldSceneChange(Core::Scene* scene);

		virtual void Draw() override final;
		void DrawEntityCreationWizard();
		void DrawEntityRenameWizard(Core::NameComponent* nameComp);

	private:
		Core::OrthographicCamera* m_editorCamera;
		Core::Scene*              m_scene;
		Core::SceneManager*       m_sceneManager;
	};
}