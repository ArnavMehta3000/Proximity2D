#include "enginepch.h"
#include "Engine/World/Entity.h"

namespace Proximity::Core
{
	Entity::Entity()
		:
		m_enttHandle(entt::null),
		m_scene(nullptr)
	{}

	Entity::Entity(entt::entity handle, Scene * scene)
		:
		m_enttHandle(handle),
		m_scene(scene)
	{}

	Entity::~Entity()
	{}


}