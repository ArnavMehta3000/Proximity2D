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
			Material,
		};

	public:
		AssetInfoPanel();
		~AssetInfoPanel();

	private:
		virtual void Draw() override;
		void OnSelectedShaderChanged(std::string_view shader) { m_showType = ShowAssetType::Shader; }
		void OnSelectedMaterialChanged(std::string_view material) { m_showType = ShowAssetType::Material; }

		void DrawSelectedShaderInfo();
		void DrawSelectedMaterialInfo();

	private:
		ShowAssetType m_showType;
		Modules::ShaderLibrary* m_shaderLib;
		Modules::MaterialLibrary* m_materialLib;
	};
}