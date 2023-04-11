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
			Texture,
			Audio
		};

	public:
		AssetInfoPanel();
		~AssetInfoPanel();

	private:
		virtual void Draw() override;
		void OnSelectedShaderChanged(std::string_view shader)     noexcept { m_showType = ShowAssetType::Shader; }
		void OnSelectedMaterialChanged(std::string_view material) noexcept { m_showType = ShowAssetType::Material; }
		void OnSelectedTextureChanged(std::string_view material)  noexcept { m_showType = ShowAssetType::Texture; }
		void OnSelectedAudioChanged(std::string_view material)    noexcept { m_showType = ShowAssetType::Audio; }

		void DrawSelectedShaderInfo();
		void DrawSelectedMaterialInfo();
		void DrawSelectedTextureInfo();
		void DrawSelectedAudioInfo(); 


		// Draws ui visuals depending on data type. Returns true if any variable was modified
		bool DrawShaderVarByType(const Graphics::MaterialVariable& var);

	private:
		ShowAssetType             m_showType;
		Modules::ShaderLibrary*   m_shaderLib;
		Modules::MaterialLibrary* m_materialLib;
		Modules::TextureLibrary*  m_textureLib;
		Modules::AudioLibrary*    m_audioLib;
		Graphics::Renderer2D* m_renderer2D;
	};
}