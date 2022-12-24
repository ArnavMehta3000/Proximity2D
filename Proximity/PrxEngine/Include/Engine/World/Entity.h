#pragma once
#include "Engine/World/Scene.h"

namespace Proximity::Core
{
	struct NameComponent;
	struct TransformComponent;

	class Entity
	{
	public:
		Entity();
		Entity(entt::entity handle, Scene* scene);
		~Entity();

		template <typename T>
		bool HasComponent() { return m_scene->m_sceneRegistry.any_of<T>(m_enttHandle); }

		template <typename T>
		void RemoveComponent()
		{ 
			using namespace Proximity::Core;
			// Don't allow to remove name or transform component
			if (std::is_same<T, NameComponent> || std::is_same<T, TransformComponent>)
				return;

			m_scene->m_sceneRegistry.remove<T>(m_enttHandle); 
		}

		template <typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_scene->m_sceneRegistry.emplace<T>(m_enttHandle, std::forward<Args>(args)...); 
		}

		template <typename T, typename... Args>
		T& GetComponent(Args&&... args)
		{
			return m_scene->m_sceneRegistry.get<T>(m_enttHandle, std::forward<Args>(args)...);
		}

	private:
		entt::entity m_enttHandle;
		Scene* m_scene;
	};
}