#pragma once
#include "entt/entt.hpp"
#include "Engine/Game/OrthographicCamera.h"
#include "Graphics/GPUBuffer.h"
#include "Graphics/Rendering/Renderer2D.h"
#include "Physics/ContactListener.h"
#include "Input/InputSystem.h"
#include <filesystem>

namespace Proximity::Modules { class SceneSerializer; }

namespace Proximity::Core
{
	class Entity;
	

	class Scene
	{
		friend class Entity;
		friend class SceneManager;
		friend class Modules::SceneSerializer;

	public:
		Scene(std::string_view name = "UntitledScene", std::filesystem::path scenePath = "");
		
		const std::filesystem::path& GetFilePath()  const noexcept { return m_scenePath; }
		const std::string            GetName()      const noexcept { return m_viewName; }

		void CreateEntity(std::string_view name = "Entity");
		void RemoveEntity(entt::entity entity);

		void SetSelectedEntity(entt::entity& selected);
		void ClearSelectedEntity() noexcept { m_currentlySelectedEntity = entt::null; }
		entt::entity& GetSelectedEntity();


		void OnScenePlay();
		void OnSceneStop();

		
		void Rename(std::string_view name);
		void OnUpdate(Math::F32 dt);
		
		// Call this render function to render the scene using the scene's camera
		void OnRender();
		// Call this render fuinction to render the scene using an external camera
		void OnRender(const Core::OrthographicCamera& cam);


	public:
		entt::registry                               m_sceneRegistry;
		std::array<float, 4>                         m_clearColor;
		b2World*                                     m_physicsWorld;

	private:
		Graphics::Renderer2D*                        m_renderer2D;
		entt::entity                                 m_currentlySelectedEntity;
		std::string                                  m_viewName;
		std::filesystem::path                        m_scenePath;
		Graphics::GPUBufferPtr<Buffers::WVPMatrices> m_camMatrices;
		Core::OrthographicCamera                     m_sceneCamera;
		Physics::ContactListener                     m_contactListener;

	};


	class SceneManager : public Utils::IService
	{
		friend class Modules::SceneSerializer;
		using ScenePathList = std::vector<std::filesystem::path>;
	public:
		SceneManager();
		~SceneManager();

		Utils::Action<Scene*> OnSceneLoadOrChanged;

		const ScenePathList& GetScenePathList() const noexcept { return m_scenePathList; }
		inline Scene* GetActiveScene() const noexcept { return m_activeScene; }

		void InitProjectLib();
		bool CreateScene(std::string_view name);
		void LoadScene(const std::string& name);	

	private:
		// Holds complete path of scene files
		ScenePathList m_scenePathList;
		Scene*        m_activeScene;
	};
}