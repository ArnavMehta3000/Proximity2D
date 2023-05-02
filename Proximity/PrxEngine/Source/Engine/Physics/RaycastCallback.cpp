#include "enginepch.h"
#include "Physics/RaycastCallback.h"

namespace Proximity::Physics
{
	RaycastManifold::RaycastManifold(bool hitAny,const std::string name, float distance, Math::Vector3 normal, Math::Vector3 point)
		:
		m_HitAny(hitAny),
		m_HitName(name),
		m_Distance(distance),
		m_Normal(normal),
		m_Point(point)
	{}

	RaycastCallback::RaycastCallback()
		:
		m_fixture(nullptr),
		m_fraction(0.0f)
	{
	}
	
	float RaycastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
	{
		m_fixture  = fixture;
		m_point    = point;
		m_normal   = normal;
		m_fraction = fraction;

		// Return the fraction to control the raycast
		return fraction;
	}
}