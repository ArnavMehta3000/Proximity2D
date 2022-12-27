#pragma once
#include "entt/entt.hpp"

namespace Proximity::Core
{
	class Entity;

	class Scene
	{
		friend class Entity;
		friend class SceneManager;
		// TODO: Add support for cameras

	public:
		Scene(std::string_view name = "UntitledScene");
		
		const std::string GetFilePath()  const noexcept { return m_scenePath; }
		const std::string GetName()      const noexcept { return m_viewName; }
		
		void Rename(std::string_view name);
		void OnUpdate(Math::F32 dt);
		void OnRender();

	private:
		void Load();
		void Unload();
	private:
		std::string    m_viewName;
		std::string    m_scenePath;
		entt::registry m_sceneRegistry;
	};


	class SceneManager : public Utils::IService
	{
		// TODO: Add support for loading/saving scenes to files
		// TODO: Load/display scenes from file path
		using ScenePathList = std::vector<std::string>;
	public:
		SceneManager() = default;
		~SceneManager();

		bool Load();
		void Unload();

		void CreateScene(std::string_view name);
		void SetScene(const std::string& name);

		void CreateEntityInActiveScene();

	private:
		ScenePathList m_scenePathList;
		Scene* m_activeScene;
	};
}