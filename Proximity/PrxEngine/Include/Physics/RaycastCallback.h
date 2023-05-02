#pragma once
#include "Box2D/Box2D.h"

namespace Proximity::Physics
{
    struct RaycastManifold
    {
        RaycastManifold(bool hitAny, const std::string name, float distance, Math::Vector3 normal, Math::Vector3 point);

        bool          m_HitAny;
        std::string   m_HitName;
        Math::F32     m_Distance;
        Math::Vector3 m_Normal;
        Math::Vector3 m_Point;
    };

	class RaycastCallback : public b2RayCastCallback
	{
    public:
        RaycastCallback();

        float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;

        b2Fixture* m_fixture;
        b2Vec2 m_point;
        b2Vec2 m_normal;
        float m_fraction;
	};
}