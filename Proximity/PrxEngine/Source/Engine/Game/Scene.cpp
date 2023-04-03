#include "enginepch.h"
#include "Engine/Game/Scene.h"
#include "Engine/Game/Entity.h"
#include "Engine/Components/Components.h"

namespace Proximity::Core
{
	Scene::Scene(std::string_view name, std::filesystem::path scenePath)
		:
		m_viewName(name),
		m_scenePath(scenePath),
		m_currentlySelectedEntity(entt::null),
		m_sceneCamera()
	{
		m_clearColor = { 0.01f, 0.01f, 0.015f , 1.0f };

		auto d3d = PRX_RESOLVE(Graphics::D3DManager);
		m_camMatrices.reset(Graphics::GPUBuffer<Buffers::WVPMatrices>::Create(d3d->GetDevice(), d3d->GetContext()));

		m_renderer2D = PRX_RESOLVE(Graphics::Renderer2D);
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

	void Scene::RemoveEntity(entt::entity entity)
	{
		m_sceneRegistry.destroy(entity);
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
		m_camMatrices->Data.View       = m_sceneCamera.GetViewMatrix().Transpose();
		m_camMatrices->Data.Projection = m_sceneCamera.GetProjectionMatrix().Transpose();

		// Get view of entities containing sprite and transform component
		auto view = m_sceneRegistry.view<Core::SpriteRendererComponent, Core::TransformComponent>();

		// Loop through the view and render
		m_renderer2D->BeginBatch();
		std::for_each(view.begin(), view.end(),
			[&](const entt::entity e)
			{
				Core::SpriteRendererComponent& mat = view.get<Core::SpriteRendererComponent>(e);
				// If material is nulltpr, then return (no shader attached to render with)
				if (mat.Material == nullptr)
					return;

				mat.Material->Apply();
				PRX_RESOLVE(Graphics::D3DManager)->GetContext()->VSSetConstantBuffers(0, 1, m_camMatrices->GetBuffer().GetAddressOf());
				Core::TransformComponent& transform = view.get<Core::TransformComponent>(e);
				
				m_camMatrices->Data.World = transform.GetWorldMatrix().Transpose();

				m_camMatrices->ApplyChanges();
				m_renderer2D->DrawQuad();
			});
		m_renderer2D->EndBatch();
	}

	void Scene::OnRender(const Core::OrthographicCamera& cam)
	{
		m_camMatrices->Data.View       = cam.GetViewMatrix().Transpose();
		m_camMatrices->Data.Projection = cam.GetProjectionMatrix().Transpose();

		// Get view of entities containing sprite and transform component
		auto view = m_sceneRegistry.view<Core::SpriteRendererComponent, Core::TransformComponent>();

		// Loop through the view and render
		m_renderer2D->BeginBatch();
		std::for_each(view.begin(), view.end(),
			[&](const entt::entity e)
			{
				Core::SpriteRendererComponent& mat = view.get<Core::SpriteRendererComponent>(e);
				// If material is nulltpr, then return (no shader attached to render with)
				if (mat.Material == nullptr)
					return;

				mat.Material->Apply();
				PRX_RESOLVE(Graphics::D3DManager)->GetContext()->VSSetConstantBuffers(0, 1, m_camMatrices->GetBuffer().GetAddressOf());
				Core::TransformComponent& transform = view.get<Core::TransformComponent>(e);

				m_camMatrices->Data.World = transform.GetWorldMatrix().Transpose();

				m_camMatrices->ApplyChanges();

				m_renderer2D->DrawQuad();
			});
		m_renderer2D->EndBatch();
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

}