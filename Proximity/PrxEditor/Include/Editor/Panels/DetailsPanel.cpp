#include "editorpch.h"
#include "Editor/Panels/DetailsPanel.h"
#include "Graphics/Material.h"

#define DEBUG_COMP 1

#if DEBUG_COMP
#define DRAW_COMPONENT_DATA(component, treeName)\
if (ImGui::TreeNode(treeName))\
{\
	ImGui::Text("Component ID: %u", component.m_ComponentID);\
	ImGui::Text("Component View Name: %s", component.m_ViewName.c_str());\
	ImGui::TreePop();\
}
#else
#define DRAW_COMPONENT_DATA(component, treeName)
#endif // DEBUG_COMP




namespace Proximity::Editor::Panels
{
	DetailsPanel::DetailsPanel()
		:
		EditorPanel("Details"),
		m_scene(nullptr)
	{
		m_sceneManager = PRX_RESOLVE(Core::SceneManager);
		m_matLib       = PRX_RESOLVE(Modules::MaterialLibrary);

		m_sceneManager->OnSceneLoadOrChanged += PRX_ACTION_FUNC(DetailsPanel::OnWorldSceneChange);
	}

	DetailsPanel::~DetailsPanel()
	{
		m_sceneManager->OnSceneLoadOrChanged -= PRX_ACTION_FUNC(DetailsPanel::OnWorldSceneChange);
	}

	void DetailsPanel::OnWorldSceneChange(Core::Scene* scene)
	{
		m_scene = scene;
	}
	
	void DetailsPanel::Draw()
	{
		//PRX_ASSERT_MSG(m_scene == nullptr, "ScenePanel::Draw() - scene is nullptr");
		if (m_scene == nullptr)
		{
			ImGui::TextColored({ 1, 1, 0, 1 }, "No scene open");
			return;
		}
		if (m_scene->GetSelectedEntity() == entt::null)
		{
			ImGui::TextColored({ 1, 1, 0, 1 }, "No entity selected");
			return;
		}

		// Cycle through components on the entity
		auto& selected = m_scene->GetSelectedEntity();

		TryShowNameComponent(selected);
		TryShowTransformComponent(selected);
		TryShowSpriteRendererComponent(selected);
	}

	void DetailsPanel::TryShowNameComponent(entt::entity& e)
	{
		auto& nameComp = m_scene->m_sceneRegistry.get<Core::NameComponent>(e);
		if (ImGui::CollapsingHeader("Data##details"))
		{
			DRAW_COMPONENT_DATA(nameComp, "Component Data##NameComponent")
			ImGui::Text("Entity Name: %s", nameComp.m_EntityName.c_str());
		}
	}

	void DetailsPanel::TryShowTransformComponent(entt::entity& e)
	{
		auto& transformComp = m_scene->m_sceneRegistry.get<Core::TransformComponent>(e);
		if (ImGui::CollapsingHeader("Transform##Details"))
		{
			DRAW_COMPONENT_DATA(transformComp, "Component Data##TransformComponent")
			ImGui::DragFloat3("Position##Transform", &transformComp.m_Position.x, 0.01f);
			ImGui::DragFloat("Rotation##Transform", &transformComp.m_Rotation, 0.01f);
			ImGui::DragFloat3("Scale##Transform", &transformComp.m_Scale.x, 0.01f);
		}
	}


	static bool showAsCol = false;
	void DetailsPanel::TryShowSpriteRendererComponent(entt::entity& e)
	{
		// Check if entity has sprite renderer
		if (!m_scene->m_sceneRegistry.all_of<Core::SpriteRendererComponent>(e))
			return;

		auto& srComp = m_scene->m_sceneRegistry.get<Core::SpriteRendererComponent>(e);
		

		if (ImGui::CollapsingHeader("Sprite Renderer##Setails"))
		{
			DRAW_COMPONENT_DATA(srComp, "Component Data##SpriteRendererComponent");

			
			if (ImGui::BeginCombo("Material##SpriteRendererComponent", srComp.Material ? srComp.Material->GetName().c_str() : "Choose Material", ImGuiComboFlags_PopupAlignLeft))
			{
				if (ImGui::Selectable("-- Clear --"))
					srComp.Material.reset();

				for (auto& pair : m_matLib->GetMap())
				{
					if (ImGui::Selectable(pair.first.c_str()))
						srComp.Material = m_matLib->Get(pair.first);
				}
				ImGui::EndCombo();
			}

			// Don't render anything beyond if there is no material assigned
			if (srComp.Material == nullptr)
				return;

			auto& cbList = srComp.Material->GetConstantBufferList();
			if (!cbList.empty())
				ImGui::Checkbox("Show Float3/4 as Color Edit?", &showAsCol);


			for (Math::U64 i = 0; i < cbList.size(); i++)
			{
				// Checks if the material constant buffer has been modified

				bool modifiedBuffer = false;
				auto& cb            = cbList[i];
				auto varCount       = cb.Variables.size();
				
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
	}

	bool DetailsPanel::DrawShaderVarByType(const Graphics::MaterialVariable& var)
	{
		using shVarType     = Graphics::MaterialVarType;
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