#pragma once
#include "Engine/Systems/UUID.h"
namespace Proximity::Core
{
	struct BaseComponent
	{
		BaseComponent(const char* viewName);

		std::string m_ViewName;
		Proximity::Core::UUID m_UUID;
	};
}