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
		m_textureLib  = PRX_RESOLVE(Modules::TextureLibrary);
		m_audioLib    = PRX_RESOLVE(Modules::AudioLibrary);
		m_renderer2D  = PRX_RESOLVE(Graphics::Renderer2D);

		m_shaderLib->OnShaderSelected     += PRX_ACTION_FUNC(AssetInfoPanel::OnSelectedShaderChanged);
		m_materialLib->OnMaterialSelected += PRX_ACTION_FUNC(AssetInfoPanel::OnSelectedMaterialChanged);
		m_textureLib->OnTextureSelected   += PRX_ACTION_FUNC(AssetInfoPanel::OnSelectedTextureChanged);
		m_audioLib->OnAudioSelected       += PRX_ACTION_FUNC(AssetInfoPanel::OnSelectedAudioChanged);

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

		case ShowAssetType::Texture:
			DrawSelectedTextureInfo();
			break;

		case ShowAssetType::Audio:
			DrawSelectedAudioInfo();
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
		if (mat == nullptr)
			return;
		auto linkedShaders = mat->GetShaderPair();

		ImGui::Text("Material Name: %s ", name.c_str());
		ImGui::Separator();

		ImGui::Text("Linked Vertex Shader:  %s", linkedShaders.first->GetName().c_str());
		ImGui::Text("Linked Pixel Shader:  %s", linkedShaders.second->GetName().c_str());
		
		static std::string target;
		static char slotName[20] = "";

		if (ImGui::Button("Reflect input slot by name"))
		{
			target.clear();
			ImGui::OpenPopup("Input Slot Reflection");
			memset(slotName, 0, sizeof(slotName));
		}

		// Slot reflection wizard
		if (ImGui::BeginPopupModal("Input Slot Reflection", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
		{
			bool reflect  = ImGui::InputText("Slot name##slotReflection", slotName, 20, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);

			if (ImGui::BeginCombo("##SelectTargetShader", target.empty() ? "Target Shader" : target.c_str(), ImGuiComboFlags_PopupAlignLeft))
			{
				if (ImGui::Selectable(linkedShaders.first->GetName().c_str(), ImGuiSelectableFlags_DontClosePopups))
					target = linkedShaders.first->GetName();

				if (ImGui::Selectable(linkedShaders.second->GetName().c_str(), ImGuiSelectableFlags_DontClosePopups))
					target = linkedShaders.second->GetName();
				ImGui::EndCombo();
			}

			bool disabled = strlen(slotName) == 0 || target.empty();

			if (disabled) ImGui::BeginDisabled();
			if (ImGui::Button("Reflect Slot") || reflect)
			{
				auto info = mat->ReflectInputSlotByName(slotName, target);
				if (FAILED(info.HResult))
				{
					PRX_LOG_ERROR("Slot reflection error: %s", info.Message.str().c_str());
				}
				else
				{
					PRX_LOG_INFO("Reflection output: %s", info.Message.str().c_str());
					PRX_LOG_INFO("Slot reflection successful");
				}

				ImGui::CloseCurrentPopup();
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

		// Loop through constant buffers
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
				ImGui::TreePop();
			}

			// Member variable of the buffer was modified. Update it
			if (modifiedBuffer)
				cb.ApplyBufferChanges();
		}

		if (mat->GetInputResourceCount() == 0)
			return;

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::TreeNodeEx("Input Resources", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& inputResList = mat->GetInputResourceList();
			for (int i = 0; i < inputResList.size(); i++)
			{
				switch (inputResList[i].Type)
				{
				case D3D_SIT_TEXTURE:  // Show texture library
				{
					auto texture = *inputResList[i].GetIf<std::shared_ptr<Graphics::Texture2D>>();
					std::string texName = (texture == nullptr) ? "Null Texture" : texture->Name;
					if (ImGui::BeginCombo("##ChooseMaterialTex", texName.c_str(), ImGuiComboFlags_PopupAlignLeft))
					{
						for (auto& pair : m_textureLib->GetMap())
						{
							if (ImGui::Selectable(pair.first.c_str()))
							{
								inputResList[i].SetData(pair.second);
							}
						}
						ImGui::EndCombo();
					}
				}
					break;

				case D3D_SIT_SAMPLER:  // Show sampler library
				{
					auto sampler = inputResList[i].GetIf<Graphics::SamplerState>();
					std::string samplerName = (sampler == nullptr) ? "Null Sampler" : sampler->Name;
					if (ImGui::BeginCombo("##ChooseMaterialSampler", samplerName.c_str(), ImGuiComboFlags_PopupAlignLeft))
					{
						for (auto& state : m_renderer2D->GetSamplerList())
						{
							if (ImGui::Selectable(state.Name.c_str()))
							{
								inputResList[i].SetData(state);
							}
						}
						ImGui::EndCombo();
					}
				}
					break;
				}
			}

			ImGui::TreePop();
		}
	}

	void AssetInfoPanel::DrawSelectedTextureInfo()
	{
		auto& name = m_textureLib->GetSelectedName();
		auto tex = m_textureLib->Get(name);
		// Image details
		ImGui::Text("Name: %s", name.c_str());
		ImGui::Text("Dimensions: [%ux%u]", tex->Width, tex->Height);
		
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Image((void*)tex->SRV.Get(), { 250, 250 });
	}

	void AssetInfoPanel::DrawSelectedAudioInfo()
	{
		auto& name = m_audioLib->GetSelectedName();
		auto src   = m_audioLib->Get(name);
		
		bool hasInstance = src->HasInstance();
		if (ImGui::Button(hasInstance ? "Destroy Instance" : "Create Instance"))
		{
			if (hasInstance)
			{
				src->DestroyInstance();
				PRX_LOG_INFO("Destroyed audio instance!");
			}
			else
			{
				src->CreateIntance();
				PRX_LOG_INFO("Created audio instance!");
			}
		}
				

		
		ImGui::Text("Name: %s", name.c_str());
		ImGui::Text("Path: %s", src->Filename.c_str());

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Channel Count: %u", src->GetChannelCount());
		ImGui::Text("Sample Duration: %ums", src->GetSampleDurationMS());
		ImGui::Text("Sample Size: %u Bytes", src->GetSampleSizeInBytes());
		ImGui::Text("Has Instance: %s", (src->HasInstance()) ? "True" : "False");
		ImGui::Spacing();
		ImGui::Text("Volume: %.2f",src->GetVolume());
		ImGui::Text("Pitch: %.2f",src->GetPitch());
		ImGui::Text("Pan: %.2f",src->GetPan());



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
