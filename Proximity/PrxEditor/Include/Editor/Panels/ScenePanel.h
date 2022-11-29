#pragma once
#include "Editor/Panels/EditorPanel.h"

namespace Proximity::Editor
{
	class ScenePanel : public EditorPanel
	{
	public:
		ScenePanel();
		virtual void Draw() override;
	};
}