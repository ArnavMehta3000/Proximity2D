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
		virtual void Draw() override final;
		void OnWorldSceneChange(const Core::Scene* scene);

		const Core::Scene* m_scene;
	};
}