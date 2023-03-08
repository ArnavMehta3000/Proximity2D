#include "editorpch.h"
#include "Editor/Panels/AssetInfoPanel.h"

namespace Proximity::Editor::Panels
{
	AssetInfoPanel::AssetInfoPanel()
		:
		EditorPanel("Asset Info"),
		m_showType(ShowAssetType::None)
	{
		m_shaderLib   = PRX_RESOLVE(Modules::ShaderLibrary);
		m_materialLib = PRX_RESOLVE(Modules::MaterialLibrary);

		m_shaderLib->OnShaderSelected     += PRX_ACTION_FUNC(AssetInfoPanel::OnSelectedShaderChanged);
		m_materialLib->OnMaterialSelected += PRX_ACTION_FUNC(AssetInfoPanel::OnSelectedMaterialChanged);

	}

	AssetInfoPanel::~AssetInfoPanel()
	{
		m_shaderLib->OnShaderSelected     -= PRX_ACTION_FUNC(AssetInfoPanel::OnSelectedShaderChanged);
		m_materialLib->OnMaterialSelected -= PRX_ACTION_FUNC(AssetInfoPanel::OnSelectedMaterialChanged);
	}

	void AssetInfoPanel::Draw()
	{
		switch (m_showType)
		{
		case ShowAssetType::None:
		default:
			break;

		case ShowAssetType::Shader:
			DrawSelectedShaderInfo();
			break;

		case ShowAssetType::Material:
			DrawSelectedMaterialInfo();
			break;
		}
	}

	void AssetInfoPanel::DrawSelectedShaderInfo()
	{
		auto& reflection = m_shaderLib->Get(m_shaderLib->GetSelectedName())->GetReflection();

		if (ImGui::CollapsingHeader("Shader Info##AsetInfo", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Shader Name: %s", reflection.Name.c_str());
			ImGui::Text("Shader Version: %u", reflection.Version);
			ImGui::Text("Shader Creator: %s", reflection.Creator.c_str());
			ImGui::Separator();
			ImGui::Text("Constant Buffer Count: %u", reflection.ConstantBuffersCount);
			ImGui::Text("Bound Resources Count: %u", reflection.BoundResources);
			ImGui::Text("Input Parameters Count: %u", reflection.InputParameters);
			ImGui::Separator();
			ImGui::Text("Texture Load Instruction Count: %u", reflection.TextureLoadInstructions);
			ImGui::Text("Texture Comparison Instruction Count: %u", reflection.TextureCompInstructions);
			ImGui::Text("Texture Bias Instruction Count: %u", reflection.TextureBiasInstructions);
			ImGui::Text("Texture Gradiant Instruction Count: %u", reflection.TextureGradientInstructions);
			ImGui::Text("Texture Store Instruction Count: %u", reflection.TextureStoreInstructions);
			ImGui::Text("Non-Categorized Texture Instruction Count: %u", reflection.TextureNormalInstructions);
			ImGui::Separator();
			ImGui::Text("Float Instruction Count: %u", reflection.FloatInstructionCount);
			ImGui::Text("Integer Instruction Count: %u", reflection.IntInstructionCount);
			ImGui::Text("U-Integer Instruction Count: %u", reflection.UintInstructionCount);
			ImGui::Text("Static Flow Control Count: %u", reflection.StaticFlowControlCount);
			ImGui::Text("Dynmaic Flow Control Count: %u", reflection.DynamicFlowControlCount);
		}
	}


	void AssetInfoPanel::DrawSelectedMaterialInfo()
	{
		auto& name = m_materialLib->GetSelectedName();
		auto mat   = m_materialLib->Get(name);
		auto linkedShaders = mat->GetShaderPair();

		ImGui::Text("Material Name: %s ", name.c_str());
		ImGui::Separator();

		ImGui::Text("Linked Shader 0:  %s", linkedShaders.first->GetName().c_str());
		if (linkedShaders.second != nullptr)
			ImGui::Text("Linked Shader 1:  %s", linkedShaders.second->GetName().c_str());
		else
			ImGui::Text("Linked Shader 1: [NULL]");
		
	}
}