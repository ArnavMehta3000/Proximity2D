#pragma once
#include "Engine/Components/BaseComponent.h"

namespace Proximity::Core
{
	struct TransformComponent : public BaseComponent
	{
		TransformComponent();
		TransformComponent(Math::Vec3 pos = Math::vec3(), Math::F32 rot = 0.0f, Math::Vec3 scale = Math::Vec3(1.0f));
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