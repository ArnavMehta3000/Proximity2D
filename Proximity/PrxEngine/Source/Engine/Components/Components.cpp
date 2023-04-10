#include "enginepch.h"
#include "Engine/Components/Components.h"
#include "Graphics/D3DManager.h"
#include "Engine/Modules/MaterialLibrary.h"

namespace Proximity::Core
{
#pragma region Transform Component
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

	TransformComponent::TransformComponent(Math::Vec3 pos, Math::Vector3 rot)
		:
		BaseComponent("Transform"),
		m_Position(pos),
		m_Rotation(rot),
		m_Scale(0.0f),
		m_parentHandle(entt::null)
	{}

	TransformComponent::TransformComponent(Math::Vec3 pos, Math::Vector3 rot, Math::Vec3 scale)
		:
		BaseComponent("Transform"),
		m_Position(pos),
		m_Rotation(rot),
		m_Scale(scale),
		m_parentHandle(entt::null)
	{}

	Math::Matrix TransformComponent::GetWorldMatrix() const noexcept
	{
		auto pos   = Math::Matrix::Translation(m_Position);
		auto rot   = Math::Matrix::Rotation(Math::Utils::DEG2RAD * m_Rotation);
		auto scale = Math::Matrix::Scale(m_Scale);

		return scale * rot * pos;
	}
#pragma endregion


#pragma region Name Component
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
#pragma endregion


#pragma region Sprite Renderer Component


	SpriteRendererComponent::SpriteRendererComponent()
		:
		BaseComponent("Sprite Renderer"),
		Material(nullptr)
	{}
#pragma endregion


#pragma region Audio Source Component
	AudioSourceComponent::AudioSourceComponent()
		:
		BaseComponent("Audio Source")
	{
	}
#pragma endregion
}