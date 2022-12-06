#include "enginepch.h"
#include "Engine/Components/BaseComponent.h"

namespace Proximity::Core
{
	BaseComponent::BaseComponent(const char* viewName)
		:
		m_ViewName(viewName),
		m_ComponentID(UUID())
	{}
}