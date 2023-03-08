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
		void DrawMaterialWizard();

		void DrawTextureLibrary();
		void DrawShaderLibrary();
		void DrawAudioibrary();
		void DrawMaterialLibrary();

	private:
		Modules::ShaderLibrary*   m_shaderLib;
		Modules::MaterialLibrary* m_materialLib;
		Core::SceneManager*       m_sceneManager;
	};
}