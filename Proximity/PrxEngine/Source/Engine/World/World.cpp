#include "enginepch.h"
#include "Engine/World/World.h"
#include "Engine/Components/Components.h"

namespace Proximity::Core
{
	World* World::s_instance = nullptr;

	World::World()
		:
		m_activeScene(nullptr)
	{}
	
	World::~World()
	{}

	World* World::Get()
	{
		if (s_instance == nullptr)
			s_instance = new World();
		
		return s_instance;
	}

	bool World::Init()
	{
		// TODO: Read world file here
		return true;
	}

	void World::Shutdown()
	{
		for (auto& scene : m_scenes)
		{
			scene->Shutdown();
			SAFE_DELETE(scene);
		}
		
		m_scenes.clear();
	}

	void World::SetScene(const std::string& name)
	{
		int index = SceneExists(name);
		if (index != -1)
		{
			m_activeScene = m_scenes[index];
			OnSceneChange(m_activeScene);
		}
	}

	void World::SetScene(Math::U32 id)
	{
		int index = SceneExists(id);
		if (index != -1)
		{
			m_activeScene = m_scenes[index];
			OnSceneChange(m_activeScene);
		}
	}


	Math::U32 World::SceneExists(const std::string& name)
	{
		for (int i = 0; i < m_scenes.size(); i++)
		{
			if (m_scenes[i]->m_viewName == name)
				return i;
		}

		return -1;
	}

	Math::U32 World::SceneExists(Math::U32 id)
	{
		Math::U32 sceneId = -1;
		for (int i = 0; i < m_scenes.size(); i++)
		{
			if (m_scenes[i]->m_id == id)
				sceneId =  i;
		}

		return sceneId;
	}

	size_t World::CreateScene(const char* name)
	{
		Scene* s = new Scene(name);
		m_scenes.emplace_back(s);
		return s->m_id;
	}

	Entity World::CreateEntity(const std::string& name)
	{
		Entity e = Entity(m_activeScene->m_sceneRegistry.create(), m_activeScene);
		
		if (name.empty())
			e.AddComponent<NameComponent>();
		else
			e.AddComponent<NameComponent>(name);

		e.AddComponent<TransformComponent>(Math::Vec3(), 0.0f, Math::Vec3(1.0f));
		return e;
	}


}