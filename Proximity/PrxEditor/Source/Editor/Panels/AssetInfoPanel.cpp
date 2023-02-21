#include "editorpch.h"
#include "Editor/Panels/AssetInfoPanel.h"

namespace Proximity::Editor::Panels
{
	AssetInfoPanel::AssetInfoPanel()
		:
		EditorPanel("Asset Info"),
		m_showType(ShowAssetType::None)
	{
		m_shaderLib = Core::Globals::g_engineServices.ResolveService<Modules::ShaderLibrary>();
		m_shaderLib->OnShaderSelected += PRX_ACTION_FUNC(AssetInfoPanel::OnSelectedShaderChanged);

	}

	AssetInfoPanel::~AssetInfoPanel()
	{
		m_shaderLib->OnShaderSelected -= PRX_ACTION_FUNC(AssetInfoPanel::OnSelectedShaderChanged);
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
		}
	}

	void AssetInfoPanel::OnSelectedShaderChanged(std::string_view shader)
	{
		m_showType = ShowAssetType::Shader;
	}

	void AssetInfoPanel::DrawSelectedShaderInfo()
	{
		auto shader    = m_shaderLib->Get(m_shaderLib->GetSelectedName());
		auto& reflector = shader->GetReflector();
		auto desc      = shader->GetDesc();

		CREATE_ZERO(D3D11_SHADER_BUFFER_DESC, bufferDescription);
		CREATE_ZERO(D3D11_SHADER_VARIABLE_DESC, variableDescription);
		ID3D11ShaderReflectionConstantBuffer* constantBuffer = nullptr;
		ID3D11ShaderReflectionVariable* variable             = nullptr;
		
		if (ImGui::CollapsingHeader("Shader Info##AsetInfo", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Shader Name: %s", desc.Name.data());
			ImGui::Text("Shader Version: %u", desc.D3DDesc.Version);
			ImGui::Text("Shader Creator: %s", desc.D3DDesc.Creator);
			ImGui::Separator();
			ImGui::Text("Constant Buffer Count: %u", desc.D3DDesc.ConstantBuffers);
			ImGui::Text("Bound Resources Count: %u", desc.D3DDesc.BoundResources);
			ImGui::Text("Input Parameters Count: %u", desc.D3DDesc.InputParameters);
			ImGui::Separator();
			ImGui::Text("Texture Load Instruction Count: %u", desc.D3DDesc.TextureLoadInstructions);
			ImGui::Text("Texture Comparison Instruction Count: %u", desc.D3DDesc.TextureCompInstructions);
			ImGui::Text("Texture Bias Instruction Count: %u", desc.D3DDesc.TextureBiasInstructions);
			ImGui::Text("Texture Gradiant Instruction Count: %u", desc.D3DDesc.TextureGradientInstructions);
			ImGui::Text("Texture Store Instruction Count: %u", desc.D3DDesc.cTextureStoreInstructions);
			ImGui::Text("Non-Categorized Texture Instruction Count: %u", desc.D3DDesc.TextureNormalInstructions);
			ImGui::Separator();
			ImGui::Text("Float Instruction Count: %u", desc.D3DDesc.FloatInstructionCount);
			ImGui::Text("Integer Instruction Count: %u", desc.D3DDesc.InstructionCount);
			ImGui::Text("U-Integer Instruction Count: %u", desc.D3DDesc.UintInstructionCount);
			ImGui::Text("Static Flow Control Count: %u", desc.D3DDesc.StaticFlowControlCount);
			ImGui::Text("Dynmaic Flow Control Count: %u", desc.D3DDesc.DynamicFlowControlCount);
		}
		
		// Don't show constant buffers
		if (desc.D3DDesc.ConstantBuffers == 0)
			return;

		if (ImGui::CollapsingHeader("Shader Constant Buffers##AsetInfo", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// Cycle through all constant buffers
			for (UINT i = 0; i < desc.D3DDesc.ConstantBuffers; i++)
			{
				constantBuffer = reflector->GetConstantBufferByIndex(i);
				constantBuffer->GetDesc(&bufferDescription);

				if (ImGui::TreeNode("Costant Buffer Name %s", bufferDescription.Name))
				{
					ImGui::Text("Constant Buffer Size: %u", bufferDescription.Size);
					ImGui::Separator();

					// Cycle through all elements of a constant buffer
					for (UINT j = 0; j < bufferDescription.Variables; j++)
					{
						variable = constantBuffer->GetVariableByIndex(j);
						variable->GetDesc(&variableDescription);

						ImGui::Text("Variable Name: %s", variableDescription.Name);
						ImGui::Text("Variable Size: %u", variableDescription.Size);
						ImGui::Text("Variable Start Offset: %u", variableDescription.StartOffset);
						ImGui::Separator();
					}
					ImGui::TreePop();
				}
			}
		}
	}
}