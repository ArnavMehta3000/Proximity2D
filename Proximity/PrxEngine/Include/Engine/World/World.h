#pragma once
#include "Engine/World/Scene.h"
#include "Engine/World/Entity.h"

namespace Proximity::Core
{
	// Similar to Unreal Engine's game instance class (constains all the scenes)
	class World
	{
	public:
		bool Init();
		void Shutdown();

		static World* Get();

		Scene* GetActiveScene() const noexcept { return m_activeScene; }
		const std::vector<Scene*>& GetSceneList() const noexcept { return m_scenes; }

		Utils::Action<Scene*> OnSceneChange;

		void SetScene(const std::string& name);
		void SetScene(Math::U32 id);

		size_t CreateScene(const char* name);
		Entity CreateEntity(const std::string& name = "");

	private:
		World();
		~World();

		Math::U32 SceneExists(const std::string& name);
		Math::U32 SceneExists(Math::U32 id);


	private:
		static World* s_instance;
		std::vector<Scene*> m_scenes;
		Scene* m_activeScene;
	};

#define WORLD Proximity::Core::World::Get()
}