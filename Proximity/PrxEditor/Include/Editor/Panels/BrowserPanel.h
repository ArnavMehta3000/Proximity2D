#pragma once
#include "Editor/Panels/EditorPanel.h"

namespace Proximity::Editor::Panels
{
	class ScriptEditorPanel;

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
		void DrawScriptsLibrary();
		void DrawShaderLibrary();
		void DrawAudioibrary();
		void DrawMaterialLibrary();

	private:
		Modules::ShaderLibrary*   m_shaderLib;
		Modules::MaterialLibrary* m_materialLib;
		Modules::TextureLibrary*  m_textureLib;
		Modules::AudioLibrary*    m_audioLibrary;
		Modules::ScriptLibrary*   m_scriptLibrary;
		Core::SceneManager*       m_sceneManager;
	};
}