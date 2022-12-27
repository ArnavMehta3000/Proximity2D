#include "enginepch.h"
#include "Engine/Game/Scene.h"
#include "Engine/Game/Entity.h"
#include "Engine/Components/Components.h"

namespace Proximity::Core
{
	Scene::Scene(std::string_view name, std::filesystem::path scenePath)
		:
		m_viewName(name),
		m_scenePath(scenePath)
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

	Scene* Scene::Load(const FilePath& scenePath)
	{
		return new Scene(Utils::DirectoryManager::GetFileNameFromDir(scenePath, false), scenePath);
	}

	void Scene::Unload(Scene* scene)
	{
		if (scene == nullptr)
			return;

		SAFE_DELETE(scene);
	}
	
	
	
	
	SceneManager::SceneManager()
		:
		m_activeScene(nullptr),
		OnSceneLoadOrChanged()
	{}

	SceneManager::~SceneManager()
	{
		m_scenePathList.clear();
		Scene::Unload(m_activeScene);
	}

	bool SceneManager::CreateScene(std::string_view name)
	{
		std::string sceneName = std::string(name).append(".prx");
		FilePath scenePath = Utils::DirectoryManager::s_appDirectories.ScenesPath / sceneName;

		auto it = std::find(m_scenePathList.begin(), m_scenePathList.end(), scenePath);
		if (it == m_scenePathList.end())
		{
			// Did not find scene with same name/path
			m_scenePathList.push_back(scenePath);
			return true;
		}
		else
			return false;
	}

	void SceneManager::LoadScene(const std::string& name)
	{
		// TODO: Fix scene serialization
		std::string sceneName = std::string(name).append(".prx");
		FilePath scenePath = Utils::DirectoryManager::s_appDirectories.ScenesPath / sceneName;
		
		auto it = std::find(m_scenePathList.begin(), m_scenePathList.end(), scenePath);
		if (it != m_scenePathList.end())
		{
			// Found scene
			m_activeScene = Scene::Load(*it);
			PRX_LOG_INFO("Loaded scene: %s", name.c_str());
			OnSceneLoadOrChanged(m_activeScene);
		}
		else
			PRX_LOG_INFO("Scene not found");
	}

	void SceneManager::CreateEntityInActiveScene()
	{
		Entity e = Entity(m_activeScene->m_sceneRegistry.create(), m_activeScene);

		e.AddComponent<NameComponent>("NewEntity");
		e.AddComponent<TransformComponent>(Math::Vec3(), 0.0f, Math::Vec3(1.0f));
	}
}