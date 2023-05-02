#include "enginepch.h"
#include "Engine/Game/ContactListener.h"
#include "Engine/Components/Components.h"
#include "Scripting/ScriptLink.h"

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

		auto nameCompA = static_cast<Core::NameComponent*>(bodyA->GetUserData());
		auto nameCompB = static_cast<Core::NameComponent*>(bodyB->GetUserData());
		
		auto linkA = (fixtureA->GetUserData() == nullptr) ? nullptr : static_cast<ScriptLink*>(fixtureA->GetUserData());
		auto linkB = (fixtureB->GetUserData() == nullptr) ? nullptr : static_cast<ScriptLink*>(fixtureB->GetUserData());

		PRX_LOG_DEBUG("COLLISION");

		if (linkA)
			linkA->CallOnCollisionStart(nameCompB->m_EntityName);

		if (linkB)
			linkB->CallOnCollisionStart(nameCompA->m_EntityName);
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

		auto nameCompA = static_cast<Core::NameComponent*>(bodyA->GetUserData());
		auto nameCompB = static_cast<Core::NameComponent*>(bodyB->GetUserData());

		auto linkA = (fixtureA->GetUserData() == nullptr) ? nullptr : static_cast<ScriptLink*>(fixtureA->GetUserData());
		auto linkB = (fixtureB->GetUserData() == nullptr) ? nullptr : static_cast<ScriptLink*>(fixtureB->GetUserData());

		if (linkA)
			linkA->CallOnCollisionEnd(nameCompA->m_EntityName);

		if (linkB)
			linkB->CallOnCollisionEnd(nameCompB->m_EntityName);
	}
}