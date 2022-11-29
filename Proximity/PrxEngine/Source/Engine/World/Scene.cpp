#include "enginepch.h"
#include "Engine/World/Scene.h"

namespace Proximity::Core
{
	Scene::Scene(const char* name)
		:
		m_viewName(name),
		m_id(-1)
	{
		std::hash<std::string> hasher;
		m_id = hasher(m_viewName);
	}
	Scene::~Scene()
	{
	}

	bool Scene::Init()
	{
		return false;
	}

	void Scene::Shutdown()
	{
	}

	void Scene::Rename(const char* name)
	{
		m_viewName = name;
		std::hash<std::string> hasher;
		m_id = hasher(m_viewName);
	}
}