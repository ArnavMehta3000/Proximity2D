#include "enginepch.h"
#include "Physics/CollisionManifold.h"

namespace Proximity::Physics
{
	CollisionManifold::CollisionManifold(const std::string& hitName, Math::Vector3 point, Math::Vector3 normal)
		:
		m_HitName(hitName),
		m_Point(point),
		m_Normal(normal)
	{
	}
}