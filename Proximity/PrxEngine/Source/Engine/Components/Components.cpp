#include "enginepch.h"
#include "Engine/Components/Components.h"

namespace Proximity::Core
{
	TransformComponent::TransformComponent()
		:
		BaseComponent("Transform"), 
		m_Position(0.0f),
		m_Rotation(0.0f),
		m_Scale(0.0f)
	{}

	TransformComponent::TransformComponent(Math::Vec3 pos, Math::F32 rot, Math::Vec3 scale)
		:
		BaseComponent("Transform"),
		m_Position(pos),
		m_Rotation(rot),
		m_Scale(scale)
	{}

}