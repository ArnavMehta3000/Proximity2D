#include "enginepch.h"
#include "Engine/Game/Entity.h"

namespace Proximity::Core
{
	Math::U64 Entity::s_entityCount = 0;

	Entity::Entity()
		:
		m_enttHandle(entt::null),
		m_parentScene(nullptr)
	{}

	Entity::Entity(entt::entity handle, Scene * scene)
		:
		m_enttHandle(handle),
		m_parentScene(scene)
	{}

	Entity::~Entity()
	{}


}