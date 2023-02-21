#pragma once
#include "Editor/Panels/EditorPanel.h"
#include <d3d11shader.h>

namespace Proximity::Editor::Panels
{
	class AssetInfoPanel : public EditorPanel
	{
		enum class ShowAssetType
		{
			None,
			Shader,
		};

	public:
		AssetInfoPanel();
		~AssetInfoPanel();

	private:
		virtual void Draw() override;
		void OnSelectedShaderChanged(std::string_view shader);

		void DrawSelectedShaderInfo();

	private:
		ShowAssetType m_showType;
		Modules::ShaderLibrary* m_shaderLib;
	};
}