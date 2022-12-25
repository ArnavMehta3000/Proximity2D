#pragma once
#include "Engine/Modules/UUID.h"
namespace Proximity::Core
{
	struct BaseComponent
	{
		BaseComponent(const char* viewName);

		std::string m_ViewName;
		Proximity::Core::UUID m_ComponentID;
	};
}