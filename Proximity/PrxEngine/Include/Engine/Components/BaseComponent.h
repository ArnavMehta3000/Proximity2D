#pragma once

namespace Proximity::Core
{
	struct BaseComponent
	{
		BaseComponent(const char* viewName);

		std::string m_ViewName;
	};
}