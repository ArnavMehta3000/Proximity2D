#pragma once
#include "entt/entt.hpp"
namespace Proximity::Core
{
	class Entity;

	class Scene
	{
		friend class Entity;
		friend class World;
	public:
		Scene(const char* name);
		~Scene();

		bool Init();
		void Shutdown();

		const entt::registry& GetEntityRegistery() const noexcept {return m_sceneRegistry;}

		// Update name and hash id
		void Rename(const char* name);


	private:
		std::string m_viewName;
		size_t m_id;
		entt::registry m_sceneRegistry;
	};
}