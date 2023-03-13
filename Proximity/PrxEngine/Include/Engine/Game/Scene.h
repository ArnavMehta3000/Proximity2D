#pragma once
#include "entt/entt.hpp"
#include "Engine/Game/OrthographicCamera.h"
#include "Graphics/GPUBuffer.h"
#include <filesystem>

namespace Proximity::Core
{
	class Entity;

	class Scene
	{
		friend class Entity;
		friend class SceneManager;
		// TODO: Add support for cameras

	public:
		Scene(std::string_view name = "UntitledScene", std::filesystem::path scenePath = "");
		
		const std::filesystem::path& GetFilePath()  const noexcept { return m_scenePath; }
		const std::string            GetName()      const noexcept { return m_viewName; }

		void CreateEntity(std::string_view name = "Entity");

		void SetSelectedEntity(entt::entity& selected);
		entt::entity& GetSelectedEntity();

		
		void Rename(std::string_view name);
		void OnUpdate(Math::F32 dt);
		void OnRender();
		void OnRender(const Core::OrthographicCamera& cam);

		static Scene* Load(const std::filesystem::path& scenePath);
		static void Unload(Scene* scene);

	public:
		entt::registry        m_sceneRegistry;

	private:
		entt::entity                                 m_currentlySelectedEntity;
		std::string                                  m_viewName;
		std::filesystem::path                        m_scenePath;
		Graphics::GPUBufferPtr<Buffers::WVPMatrices> m_camMatrices;
	};


	class SceneManager : public Utils::IService
	{
		// TODO: Add support for loading/saving scenes to files
		// TODO: Load/display scenes from file path
		using ScenePathList = std::vector<std::filesystem::path>;
	public:
		SceneManager();
		~SceneManager();

		Utils::Action<Scene*> OnSceneLoadOrChanged;

		const ScenePathList& GetScenePathList() const noexcept { return m_scenePathList; }
		inline Scene* GetActiveScene() const noexcept { return m_activeScene; }

		bool CreateScene(std::string_view name);
		void LoadScene(const std::string& name);

	private:
		// Holds complete path of scene files
		ScenePathList m_scenePathList;
		Scene* m_activeScene;
	};
}