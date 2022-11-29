#include "enginepch.h"
#include "Engine/Components/Components.h"

namespace Proximity::Core
{
	TransformComponent::TransformComponent()
		:
		BaseComponent("Transform"), 
		m_Position(0.0f),
		m_Rotation(0.0f),
		m_Scale(0.0f),
		m_parentHandle(entt::null)
	{}

	TransformComponent::TransformComponent(Math::Vec3 pos)
		:
		BaseComponent("Transform"),
		m_Position(pos),
		m_Rotation(0.0f),
		m_Scale(0.0f),
		m_parentHandle(entt::null)
	{}

	TransformComponent::TransformComponent(Math::Vec3 pos, Math::F32 rot)
		:
		BaseComponent("Transform"),
		m_Position(pos),
		m_Rotation(rot),
		m_Scale(0.0f),
		m_parentHandle(entt::null)
	{}

	TransformComponent::TransformComponent(Math::Vec3 pos, Math::F32 rot, Math::Vec3 scale)
		:
		BaseComponent("Transform"),
		m_Position(pos),
		m_Rotation(rot),
		m_Scale(scale),
		m_parentHandle(entt::null)
	{}



	NameComponent::NameComponent()
		:
		BaseComponent("Name"),
		m_EntityName("Unnamed Entity")
	{}

	NameComponent::NameComponent(const std::string& name)
		:
		BaseComponent("Name"),
		m_EntityName(name)
	{}

}