#include "enginepch.h"
#include "Engine/Game/Scene.h"
#include "Engine/Game/Entity.h"
#include "Engine/Components/Components.h"
#include "Graphics/Rendering/Renderer2D.h"

namespace Proximity::Core
{
	Scene::Scene(std::string_view name, std::filesystem::path scenePath)
		:
		m_viewName(name),
		m_scenePath(scenePath),
		m_currentlySelectedEntity(entt::null)
	{
	}
	
	void Scene::CreateEntity(std::string_view name)
	{
		Entity e = Entity(m_sceneRegistry.create(), this);

		// Check if any other entity has the same name
		bool nameFound = false;
		std::string setName(name.data());
		Math::U64 nameFoundCount = Entity::s_entityCount;

		auto view = m_sceneRegistry.view<Core::NameComponent>();
		std::for_each(view.begin(), view.end(),
			[&](entt::entity e)
			{
				// Get name component on existin entities
				auto& nameComp = m_sceneRegistry.get<Core::NameComponent>(e);
				if (!nameComp.m_EntityName.compare(name))
				{
					nameFound = true;
					nameFoundCount++;
				}
			});
		Entity::s_entityCount = nameFoundCount;

		if (nameFound)
			setName.append(std::to_string(nameFoundCount));

		e.AddComponent<NameComponent>(setName.data());
		e.AddComponent<TransformComponent>(Math::Vec3(), 0.0f, Math::Vec3(1.0f));
		e.AddComponent<SpriteRendererComponent>();
	}

	void Scene::SetSelectedEntity(entt::entity& selected)
	{
		m_currentlySelectedEntity = selected;
	}

	entt::entity& Scene::GetSelectedEntity()
	{
		return m_currentlySelectedEntity;
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
		auto view = m_sceneRegistry.view<Core::SpriteRendererComponent>();
		for (int i = 0; i < view.size(); i++)
		{
			auto entity = view[i];
			PRX_RESOLVE(Graphics::Renderer2D)->DrawQuad();
		}
	}

	void Scene::OnRender(const Core::OrthographicCamera& cam)
	{
	}

	Scene* Scene::Load(const FilePath& scenePath)
	{
		throw Proximity::Execptions::MethodNotImplemented("Scene Loading not implemented");/*new Scene(Utils::DirectoryManager::GetFileNameFromDir(scenePath, false), scenePath);*/
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

}