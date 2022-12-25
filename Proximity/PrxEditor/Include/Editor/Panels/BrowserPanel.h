#pragma once
#include "Editor/Panels/EditorPanel.h"


namespace Proximity::Editor::Panels
{
	class BrowswerPanel : public EditorPanel
	{
	public:
		BrowswerPanel();

	private:
		virtual void Draw() override final;
		void DrawTextureLibrary();
		void DrawShaderLibrary();
		void DrawAudioibrary();

	private:
		Modules::ShaderLibrary* m_shaderLib;
	};
}