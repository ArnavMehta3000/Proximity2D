#pragma once
#include "Editor/Panels/EditorPanel.h"

namespace Proximity::Editor::Panels
{
	class BrowserPanel : public EditorPanel
	{
	public:
		BrowserPanel();

	private:
		virtual void Draw() override final;

		void DrawSceneLibrary();
		void DrawSceneList();
		void DrawSceneWizard();

		void DrawTextureLibrary();
		void DrawShaderLibrary();
		void DrawAudioibrary();

	private:
		Modules::ShaderLibrary* m_shaderLib;
		Core::SceneManager* m_sceneManager;
	};
}