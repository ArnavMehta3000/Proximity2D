#include "enginepch.h"
#include "Physics/ContactListener.h"
#include "Engine/Components/Components.h"
#include "Scripting/ScriptLink.h"
#include "Physics/CollisionManifold.h"

namespace Proximity::Physics
{
	ContactListener::ContactListener() = default;

	void ContactListener::BeginContact(b2Contact* contact)
	{
		using Scripting::ScriptLink;
		// Body name component as user data
		// Fixture has script link as user data

		auto fixtureA = contact->GetFixtureA();
		auto fixtureB = contact->GetFixtureB();
		auto bodyA    = fixtureA->GetBody();
		auto bodyB    = fixtureB->GetBody();

		const auto& nameCompA = static_cast<Core::NameComponent*>(bodyA->GetUserData())->m_EntityName;
		const auto& nameCompB = static_cast<Core::NameComponent*>(bodyB->GetUserData())->m_EntityName;
		
		auto linkA = (fixtureA->GetUserData() == nullptr) ? nullptr : static_cast<ScriptLink*>(fixtureA->GetUserData());
		auto linkB = (fixtureB->GetUserData() == nullptr) ? nullptr : static_cast<ScriptLink*>(fixtureB->GetUserData());

		auto mf = contact->GetManifold();

		auto point = mf->points[0].localPoint;
		auto hitPoint = Math::Vector3(point.x, point.y, 0.0f);

		auto normal = mf->localNormal;
		auto hitNormal = Math::Vector3(normal.x, normal.y, 0.0f);

		auto mfA = CollisionManifold(nameCompA, hitPoint, hitNormal);
		auto mfB = CollisionManifold(nameCompB, hitPoint, hitNormal * -1.0f);



		if (linkA)
			linkA->CallOnCollisionStart(mfB);

		if (linkB)
			linkB->CallOnCollisionStart(mfA);
	}

	void ContactListener::EndContact(b2Contact* contact)
	{
		using Scripting::ScriptLink;
		// Body name component as user data
		// Fixture has script link as user data

		auto fixtureA = contact->GetFixtureA();
		auto fixtureB = contact->GetFixtureB();
		auto bodyA    = fixtureA->GetBody();
		auto bodyB    = fixtureB->GetBody();

		const auto& nameCompA = static_cast<Core::NameComponent*>(bodyA->GetUserData())->m_EntityName;
		const auto& nameCompB = static_cast<Core::NameComponent*>(bodyB->GetUserData())->m_EntityName;

		auto linkA = (fixtureA->GetUserData() == nullptr) ? nullptr : static_cast<ScriptLink*>(fixtureA->GetUserData());
		auto linkB = (fixtureB->GetUserData() == nullptr) ? nullptr : static_cast<ScriptLink*>(fixtureB->GetUserData());

		auto mf = contact->GetManifold();

		auto point = mf->points[0].localPoint;
		auto hitPoint = Math::Vector3(point.x, point.y, 0.0f);

		auto normal = mf->localNormal;
		auto hitNormal = Math::Vector3(normal.x, normal.y, 0.0f);

		auto mfA = CollisionManifold(nameCompA, hitPoint, hitNormal);
		auto mfB = CollisionManifold(nameCompB, hitPoint, hitNormal * -1.0f);



		if (linkA)
			linkA->CallOnCollisionStart(mfB);

		if (linkB)
			linkB->CallOnCollisionStart(mfA);
	}
}