#pragma once
#include "Box2D/Box2D.h"

namespace Proximity::Physics
{
	class ContactListener : public b2ContactListener
	{
	public:
		ContactListener();

		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;
	};
}