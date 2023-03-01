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
		m_sceneManager = Core::Globals::g_engineServices.ResolveService<Core::SceneManager>();
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

	void DetailsPanel::TryShowSpriteRendererComponent(entt::entity& e)
	{
		// Check if entity has sprite renderer
		if (!m_scene->m_sceneRegistry.all_of<Core::SpriteRendererComponent>(e))
			return;

		auto& srComp = m_scene->m_sceneRegistry.get<Core::SpriteRendererComponent>(e);
		

		if (ImGui::CollapsingHeader("Sprite Renderer##Setails"))
		{
			DRAW_COMPONENT_DATA(srComp, "Component Data##SpriteRendererComponent")

			
			auto& cbList = srComp.Material->GetConstantBufferList();

			for (Math::U64 i = 0; i < cbList.size(); i++)
			{
				if (ImGui::TreeNode("ConstantBuffers##Details"))
				{
					auto& cb = cbList[i];
					ImGui::Text("Buffer Name: %s", cb.Desc.Name);

					for (Math::U32 j = 0; j < cb.Variables.size(); j++)
					{
						auto& var = cb.Variables[j];

						DrawShaderVarByType(var);
						//ImGui::Text("Variable Type: %s", var.TypeDesc.Name);
						
						ImGui::Separator();
					}
					ImGui::TreePop();
				}
			}
		}
	}

	void DetailsPanel::DrawShaderVarByType(const Graphics::GPUShaderVariable& var)
	{
		using shVarType = Graphics::GPUShaderVarType;

		switch (var.Type)
		{
		case shVarType::BOOL:
		{
			auto valPtr = var.GetIf<bool>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader bool variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::Checkbox(var.Name.c_str(), &changeVal);
		}
		break;


		case shVarType::INT:
		{
			auto valPtr = var.GetIf<int>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader int variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragInt(var.Name.c_str(), &changeVal);
		}
		break;

		case shVarType::INT2:
		{
			auto valPtr = var.GetIf<DirectX::XMINT2>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader int2 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragInt2(var.Name.c_str(), &changeVal.x);
		}
		break;

		case shVarType::INT3:
		{
			auto valPtr = var.GetIf<DirectX::XMINT3>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader int3 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragInt3(var.Name.c_str(), &changeVal.x);
		}
		break;

		case shVarType::INT4:
		{
			auto valPtr = var.GetIf<DirectX::XMINT4>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader int4 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragInt4(var.Name.c_str(), &changeVal.x);
		}
		break;


		case shVarType::UINT:
		{
			auto valPtr = var.GetIf<UINT>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader uint variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragScalar(var.Name.c_str(), ImGuiDataType_U32, &changeVal);
		}
		break;

		case shVarType::UINT2:
		{
			auto valPtr = var.GetIf<DirectX::XMUINT2>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader uint2 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragScalarN(var.Name.c_str(), ImGuiDataType_U32, &changeVal.x, 2);
		}
		break;

		case shVarType::UINT3:
		{
			auto valPtr = var.GetIf<DirectX::XMUINT3>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader uint3 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragScalarN(var.Name.c_str(), ImGuiDataType_U32, &changeVal.x, 3);
		}
		break;

		case shVarType::UINT4:
		{
			auto valPtr = var.GetIf<DirectX::XMUINT4>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader uint4 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragScalarN(var.Name.c_str(), ImGuiDataType_U32, &changeVal.x, 4);
		}
		break;


		case shVarType::FLOAT:
		{
			auto valPtr = var.GetIf<float>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader float variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragFloat(var.Name.c_str(), &changeVal, 0.1f);
		}
		break;

		case shVarType::FLOAT2:
		{
			auto valPtr = var.GetIf<DirectX::XMFLOAT2>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader float2 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragFloat2(var.Name.c_str(), &changeVal.x, 0.1f);
		}
		break;

		case shVarType::FLOAT3:
		{
			auto valPtr = var.GetIf<DirectX::XMFLOAT3>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader float3 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragFloat3(var.Name.c_str(), &changeVal.x, 0.1f);
		}
		break;

		case shVarType::FLOAT4:
		{
			auto valPtr = var.GetIf<DirectX::XMFLOAT4>();
			PRX_ASSERT_MSG(valPtr == nullptr, "Shader float4 variant is nullptr!");

			auto changeVal = *valPtr;

			ImGui::DragFloat4(var.Name.c_str(), &changeVal.x, 0.1f);
		}
		break;

		case shVarType::Unknown:
		default:
			ImGui::Text("No draw for shader variable type: %s", var.Name.c_str());
			break;
		}
	}
}