#include "enginepch.h"
#include "Utils/ServiceLocator.h"

namespace Proximity::Utils
{
	ServiceLocator::ServiceLocator() noexcept
	{
	}

	ServiceLocator::~ServiceLocator() noexcept
	{
		m_services.clear();
	}

	bool ServiceLocator::HasService(size_t service)
	{
		if (m_services.find(service) == m_services.end())
			return false;  // not found

		return true; // found;
	}
}