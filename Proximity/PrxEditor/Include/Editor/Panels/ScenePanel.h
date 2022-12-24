#pragma once
#include "Editor/Panels/EditorPanel.h"

namespace Proximity::Editor::Panels
{
	class ScenePanel : public EditorPanel
	{
	public:
		ScenePanel();

	private:
		virtual void Draw() override final;
		void OnWorldSceneChange(Core::Scene* scene);

		Core::Scene* m_scene;
	};
}