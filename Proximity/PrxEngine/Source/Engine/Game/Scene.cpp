#include "enginepch.h"
#include "Engine/Game/Scene.h"
#include "Engine/Game/Entity.h"
#include "Engine/Components/Components.h"

namespace Proximity::Core
{
	Scene::Scene(std::string_view name)
		:
		m_viewName(name)
	{
	}
	
	void Scene::Rename(std::string_view name)
	{
		m_viewName.clear();
		m_viewName = name;
	}

	void Scene::OnUpdate(Math::F32 dt)
	{
	}

	void Scene::OnRender()
	{
	}

	void Scene::Load()
	{
	}

	void Scene::Unload()
	{
	}
	
	
	
	
	
	SceneManager::~SceneManager()
	{
		m_scenePathList.clear();
		m_activeScene->Unload();
	}

	bool SceneManager::Load()
	{
		return false;
	}

	void SceneManager::Unload()
	{
	}

	void SceneManager::CreateScene(std::string_view name)
	{
		m_scenePathList.push_back(name.data());
	}

	void SceneManager::SetScene(const std::string& name)
	{
		// find scene
	}

	void SceneManager::CreateEntityInActiveScene()
	{
		Entity e = Entity(m_activeScene->m_sceneRegistry.create(), m_activeScene);

		e.AddComponent<NameComponent>("NewEntity");
		e.AddComponent<TransformComponent>(Math::Vec3(), 0.0f, Math::Vec3(1.0f));
	}
}