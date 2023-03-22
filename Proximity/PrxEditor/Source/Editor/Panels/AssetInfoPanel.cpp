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

	static bool showAsCol = false;
	void AssetInfoPanel::DrawSelectedMaterialInfo()
	{
		auto& name = m_materialLib->GetSelectedName();
		auto mat   = m_materialLib->Get(name);
		auto linkedShaders = mat->GetShaderPair();

		ImGui::Text("Material Name: %s ", name.c_str());
		ImGui::Separator();

		ImGui::Text("Linked Vertex Shader:  %s", linkedShaders.first->GetName().c_str());
		ImGui::Text("Linked Pixel Shader:  %s", linkedShaders.second->GetName().c_str());
		
		static std::string target;
		static char slotName[20] = "";

		if (ImGui::Button("Reflect input slot by name"))
		{
			ImGui::OpenPopup("Input Slot Reflection");
			target.clear();
			memset(slotName, 0, sizeof(slotName));
		}

		// Slot reflection wizard
		if (ImGui::BeginPopupModal("Input Slot Reflection", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
		{
			bool reflect             = ImGui::InputText("Slot name##slotReflection", slotName, 20, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);
			bool disabled            = strlen(slotName) == 0;

			if (ImGui::BeginCombo("##SelectTargetShader", target.empty() ? "Target Shader" : target.c_str(), ImGuiComboFlags_PopupAlignLeft))
			{
				if (ImGui::Selectable(linkedShaders.first->GetName().c_str(), ImGuiSelectableFlags_DontClosePopups))
					target = linkedShaders.first->GetName();

				if (ImGui::Selectable(linkedShaders.second->GetName().c_str(), ImGuiSelectableFlags_DontClosePopups))
					target = linkedShaders.second->GetName();
				ImGui::EndCombo();
			}

			if (disabled) ImGui::BeginDisabled();
			if (ImGui::Button("Reflect Slot") || reflect)
			{
				auto info = mat->ReflectInputSlotByName(slotName, target);
				if (FAILED(info.HResult))
				{
					PRX_LOG_ERROR("Slot reflection error: %s", info.Message.str().c_str());
					ImGui::CloseCurrentPopup();
				}

				PRX_LOG_INFO("Slot reflection successful");
			}
			if (disabled) ImGui::EndDisabled();
			
			ImGui::SameLine();
			if (ImGui::Button("Cancel##reflection"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

		

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		
		// Draw material reflection information
		auto& cbList = mat->GetConstantBufferList();
		if (!cbList.empty())
			ImGui::Checkbox("Show Float3/4 as Color Edit?", &showAsCol);


		for (Math::U64 i = 0; i < cbList.size(); i++)
		{
			// Checks if the material constant buffer has been modified

			bool modifiedBuffer = false;
			auto& cb = cbList[i];
			auto varCount = cb.Variables.size();

			if (varCount == 0)
				continue;

			if (ImGui::Button("Revert Default"))
			{
				cb.ResetBufferValues();
				cb.ApplyBufferChanges();
				break;
			}
			ImGui::SameLine();



			if (ImGui::TreeNodeEx("##Details", ImGuiTreeNodeFlags_Bullet, "Buffer Name: %s", cb.Desc.Name))
			{
				for (Math::U32 j = 0; j < varCount; j++)
				{
					// Flag buffer for pdate if any variable was updated 
					if (DrawShaderVarByType(cb.Variables[j]))
						modifiedBuffer = true;

				}
				ImGui::Separator();
				ImGui::TreePop();
			}

			// Member variable of the buffer was modified. Update it
			if (modifiedBuffer)
				cb.ApplyBufferChanges();
		}
	}

	bool AssetInfoPanel::DrawShaderVarByType(const Graphics::MaterialVariable& var)
	{
		using shVarType = Graphics::MaterialVarType;
		UINT colorEditFlags = ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_Float;
		Graphics::ShaderVar_T updateValue;


		switch (var.Type)
		{
		case shVarType::BOOL:
		{
			auto valPtr = var.GetIf<bool>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader bool variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::Checkbox(var.Name.c_str(), &changeVal);
			updateValue = changeVal;
		}
		break;


		case shVarType::INT:
		{
			auto valPtr = var.GetIf<int>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader int variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragInt(var.Name.c_str(), &changeVal);
			updateValue = changeVal;
		}
		break;

		case shVarType::INT2:
		{
			auto valPtr = var.GetIf<DirectX::XMINT2>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader int2 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragInt2(var.Name.c_str(), &changeVal.x);
			updateValue = changeVal;
		}
		break;

		case shVarType::INT3:
		{
			auto valPtr = var.GetIf<DirectX::XMINT3>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader int3 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragInt3(var.Name.c_str(), &changeVal.x);
			updateValue = changeVal;
		}
		break;

		case shVarType::INT4:
		{
			auto valPtr = var.GetIf<DirectX::XMINT4>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader int4 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragInt4(var.Name.c_str(), &changeVal.x);
			updateValue = changeVal;
		}
		break;


		case shVarType::UINT:
		{
			auto valPtr = var.GetIf<UINT>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader uint variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragScalar(var.Name.c_str(), ImGuiDataType_U32, &changeVal);
			updateValue = changeVal;
		}
		break;

		case shVarType::UINT2:
		{
			auto valPtr = var.GetIf<DirectX::XMUINT2>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader uint2 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragScalarN(var.Name.c_str(), ImGuiDataType_U32, &changeVal.x, 2);
			updateValue = changeVal;
		}
		break;

		case shVarType::UINT3:
		{
			auto valPtr = var.GetIf<DirectX::XMUINT3>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader uint3 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragScalarN(var.Name.c_str(), ImGuiDataType_U32, &changeVal.x, 3);
			updateValue = changeVal;
		}
		break;

		case shVarType::UINT4:
		{
			auto valPtr = var.GetIf<DirectX::XMUINT4>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader uint4 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragScalarN(var.Name.c_str(), ImGuiDataType_U32, &changeVal.x, 4);
			updateValue = changeVal;
		}
		break;


		case shVarType::FLOAT:
		{
			auto valPtr = var.GetIf<float>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader float variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragFloat(var.Name.c_str(), &changeVal, 0.1f);
			updateValue = changeVal;
		}
		break;

		case shVarType::FLOAT2:
		{
			auto valPtr = var.GetIf<DirectX::XMFLOAT2>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader float2 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragFloat2(var.Name.c_str(), &changeVal.x, 0.1f);
			updateValue = changeVal;
		}
		break;

		case shVarType::FLOAT3:
		{
			auto valPtr = var.GetIf<DirectX::XMFLOAT3>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader float3 variant is nullptr!");

			auto changeVal = *valPtr;

			if (showAsCol)
				ImGui::ColorEdit3(var.Name.c_str(), &changeVal.x, colorEditFlags);
			else
				ImGui::DragFloat3(var.Name.c_str(), &changeVal.x, 0.1f);
			updateValue = changeVal;
		}
		break;

		case shVarType::FLOAT4:
		{
			auto valPtr = var.GetIf<DirectX::XMFLOAT4>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader float4 variant is nullptr!");

			auto changeVal = *valPtr;
			if (showAsCol)
				ImGui::ColorEdit4(var.Name.c_str(), &changeVal.x, colorEditFlags);
			else
				ImGui::DragFloat4(var.Name.c_str(), &changeVal.x, 0.1f);
			updateValue = changeVal;
		}
		break;

		case shVarType::Unknown:
		default:
			ImGui::Text("No draw for shader variable type: %s", var.Name.c_str());
			break;
		}

		// Check if visualized buffer value was modified
		bool edited = ImGui::IsItemEdited();
		if (edited)
			var.SetData(updateValue);


		return edited;
	}
}
