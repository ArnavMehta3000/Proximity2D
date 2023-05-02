#pragma once
#include "Engine/Game/Scene.h"

namespace Proximity::Core
{
	struct NameComponent;
	struct TransformComponent;
	struct SpriteRendererComponent;

	class Entity
	{
	public:
		Entity();
		Entity(entt::entity handle, Scene* scene);
		~Entity();

		template <typename T>
		bool HasComponent() { return m_parentScene->m_sceneRegistry.any_of<T>(m_enttHandle); }

		template <typename T>
		void RemoveComponent()
		{ 
			m_parentScene->m_sceneRegistry.remove<T>(m_enttHandle); 
		}

		template <typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_parentScene->m_sceneRegistry.emplace<T>(m_enttHandle, std::forward<Args>(args)...); 
		}

		template <typename T>
		T& GetComponent()
		{
			return m_parentScene->m_sceneRegistry.get<T>(m_enttHandle);
		}

		inline const auto GetScene() const noexcept { return m_parentScene; }

	public:
		static Math::U64 s_entityCount;

	private:
		entt::entity m_enttHandle;
		Scene*       m_parentScene;
	};
}