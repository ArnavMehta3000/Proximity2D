#pragma once
#include "entt/entt.hpp"
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
		
		void Rename(std::string_view name);
		void OnUpdate(Math::F32 dt);
		void OnRender();

		static Scene* Load(const std::filesystem::path& scenePath);
		static void Unload(Scene* scene);
	private:
		std::string           m_viewName;
		std::filesystem::path m_scenePath;
		entt::registry        m_sceneRegistry;
	};


	class SceneManager : public Utils::IService
	{
		// TODO: Add support for loading/saving scenes to files
		// TODO: Load/display scenes from file path
		using ScenePathList = std::vector<std::filesystem::path>;
	public:
		SceneManager();
		~SceneManager();

		Utils::Action<const Scene*> OnSceneLoadOrChanged;

		const ScenePathList& GetScenePathList() const noexcept { return m_scenePathList; }



		bool CreateScene(std::string_view name);
		void LoadScene(const std::string& name);

		void CreateEntityInActiveScene();

	private:
		// Holds complete path of scene files
		ScenePathList m_scenePathList;
		Scene* m_activeScene;
	};
}