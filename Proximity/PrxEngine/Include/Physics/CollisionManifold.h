#pragma once

namespace Proximity::Physics
{
	struct CollisionManifold
	{
		CollisionManifold(const std::string& hitName, Math::Vector3 point, Math::Vector3 normal);

		std::string   m_HitName;
		Math::Vector3 m_Point;
		Math::Vector3 m_Normal;
	};
}