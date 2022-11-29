#pragma once
#include "Engine/Components/BaseComponent.h"

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
		TransformComponent(Math::Vec3 pos, Math::F32 rot);
		TransformComponent(Math::Vec3 pos, Math::F32 rot, Math::Vec3 scale);
		TransformComponent(const TransformComponent&) = default;
		

		Math::Vector3 m_Position;
		Math::F32     m_Rotation;
		Math::Vector3 m_Scale;

		operator Math::Matrix () noexcept
		{
			return DX::XMMatrixScalingFromVector(m_Scale) * DX::XMMatrixRotationZ(m_Rotation) * DX::XMMatrixTranslationFromVector(m_Position);
		}

		operator const Math::Matrix() noexcept
		{
			return DX::XMMatrixScalingFromVector(m_Scale) * DX::XMMatrixRotationZ(m_Rotation) * DX::XMMatrixTranslationFromVector(m_Position);
		}
	};
}