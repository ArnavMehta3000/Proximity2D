#include "enginepch.h"
#include "Engine/Systems/UUID.h"
#include <random>
namespace Proximity::Core
{
	static std::random_device s_randomDev;
	static std::mt19937_64 s_engine(s_randomDev());
	static std::uniform_int_distribution<Math::U64> s_uniformDist;

	static std::unordered_set<UUID> m_uuidSet;

	UUID::UUID()
		:
		m_uuid(s_uniformDist(s_engine))
	{
		Math::U64 temp = s_uniformDist(s_engine);
		
		while (m_uuidSet.find(temp) != m_uuidSet.end())
			temp = s_uniformDist(s_engine);
		
		m_uuid = UUID(temp);
		m_uuidSet.insert(m_uuid);
	}

	UUID::UUID(Proximity::Math::U64 uuid)
		:
		m_uuid(uuid)
	{
	}
	std::string UUID::ToString()
	{
		return std::to_string(m_uuid);
	}
}
