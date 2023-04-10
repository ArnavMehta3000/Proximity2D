#pragma once
#include "Engine/Components/BaseComponent.h"
#include "Graphics/Material.h"
#include "Graphics/VertexStructures.h"
#include "../External/entt/entt.hpp"
#include "Engine/Audio/SoundSystem.h"

namespace Proximity::Core
{
	struct NameComponent : public BaseComponent
	{
		NameComponent();
		NameComponent(const std::string& name);
		NameComponent(const NameComponent&) = default;

		std::string m_EntityName;
	};




	struct TransformComponent : public BaseComponent
	{
		TransformComponent();
		TransformComponent(Math::Vec3 pos);
		TransformComponent(Math::Vec3 pos, Math::Vector3 rot);
		TransformComponent(Math::Vec3 pos, Math::Vector3 rot, Math::Vec3 scale);
		TransformComponent(const TransformComponent&) = default;
		
		Math::Vector3 m_Position;
		Math::Vector3 m_Rotation;
		Math::Vector3 m_Scale;
		entt::entity  m_parentHandle;

		Math::Matrix GetWorldMatrix() const noexcept;

		operator Math::Matrix () noexcept
		{
			return DX::XMMatrixScalingFromVector(m_Scale) * DX::XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z) * DX::XMMatrixTranslationFromVector(m_Position);
		}

		operator const Math::Matrix() noexcept
		{
			return DX::XMMatrixScalingFromVector(m_Scale) * DX::XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z) * DX::XMMatrixTranslationFromVector(m_Position);
		}
	};

	

	struct SpriteRendererComponent : public BaseComponent
	{
		SpriteRendererComponent();
		std::shared_ptr<Graphics::Material> Material;
	};



	struct AudioSourceComponent : public BaseComponent
	{
		AudioSourceComponent();
		std::shared_ptr<Audio::AudioSource> Source;
	};
}