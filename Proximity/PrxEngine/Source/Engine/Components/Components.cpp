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
		m_Material(nullptr)
	{}
#pragma endregion


#pragma region Audio Source Component
	AudioSourceComponent::AudioSourceComponent()
		:
		BaseComponent("Audio Source")
	{
	}
#pragma endregion


#pragma region Rigid Body 2D Component
	RigidBody2DComponent::RigidBody2DComponent()
		:
		BaseComponent("Rigid Body 2D"),
		m_Type(BodyType::Static),
		m_FixedRotation(false)
	{
	}

	std::string RigidBody2DComponent::BodyTypeToString(Core::RigidBody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Proximity::Core::RigidBody2DComponent::BodyType::Static: return "Static";
		case Proximity::Core::RigidBody2DComponent::BodyType::Dynamic: return "Dynamic";
		case Proximity::Core::RigidBody2DComponent::BodyType::Kinematic: return "Kinematic";
		}

		return "Static";
	}

	RigidBody2DComponent::BodyType RigidBody2DComponent::StringToBodyType(const std::string& bodyType)
	{
		if (bodyType == "Static" || bodyType == "STATIC" || bodyType == "static")
			return Core::RigidBody2DComponent::BodyType::Static;

		if (bodyType == "Dynamic" || bodyType == "DYNAMIC" || bodyType == "dynamic")
			return Core::RigidBody2DComponent::BodyType::Dynamic;

		if (bodyType == "Kinematic" || bodyType == "KINEMATIC" || bodyType == "kinematic")
			return Core::RigidBody2DComponent::BodyType::Kinematic;

		// Default return
		return Core::RigidBody2DComponent::BodyType::Static;
	}
#pragma endregion


#pragma region Box Collider 2D Component
	BoxCollider2DComponent::BoxCollider2DComponent()
		:
		BaseComponent("Box Collider 2D"),
		m_Density(0.1f),
		m_Friction(0.5f),
		m_Restitution(0.2f)
	{
		m_Offset[0] = 0.0f;
		m_Offset[1] = 0.0f;

		m_Size[0] = 0.5f;
		m_Size[1] = 0.5f;
	}
#pragma endregion

}