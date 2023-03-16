#include "enginepch.h"

#include "Engine/Components/Components.h"
#include "Utils/DirectoryManager.h"
#include "Engine/Modules/SceneSerializer.h"
#include "Utils/Exceptions.h"

namespace Proximity::Modules
{
	// Overload for Vector3 to yaml
	YAML::Emitter& operator<<(YAML::Emitter& out, const Math::Vector3 & v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		
		return out;
	}

	SceneSerializer::SceneSerializer(Core::Scene* scene)
		:
		m_scene(scene)
	{}

	static void SerializeBaseComponent(YAML::Emitter& out, Core::BaseComponent& base)
	{
		out << YAML::BeginMap; // Base comp
		out << YAML::Key << "Component ID" << YAML::Value << base.m_ComponentID;
		out << YAML::Key << "View Name" << YAML::Value << base.m_ViewName;
		out << YAML::EndMap;   // Base comp
	}

	static void SerializeEntity(YAML::Emitter& out, Core::Entity entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << "1234567890";  // TODO: Entity GUID
		
		if (entity.HasComponent<Core::NameComponent>())
		{
			auto& nameComp = entity.GetComponent<Core::NameComponent>();

			out << YAML::Key << "Name Component";
			out << YAML::BeginMap;  // Name Component
			SerializeBaseComponent(out, nameComp);
			out << YAML::Key << "Entity Name" << YAML::Value << nameComp.m_EntityName;
			out << YAML::EndMap;    // Name Component
		}

		if (entity.HasComponent<Core::TransformComponent>())
		{
			auto& transform = entity.GetComponent<Core::TransformComponent>();
			
			out << YAML::Key << "Transfrom Component";
			out << YAML::BeginMap;  // Transform Component
			SerializeBaseComponent(out, transform);
			out << YAML::Key << "Position" << YAML::Value << transform.m_Position;
			out << YAML::Key << "Rotation" << YAML::Value << transform.m_Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transform.m_Scale;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}
	
	bool SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << m_scene->m_viewName;
		m_scene->m_sceneRegistry.each([&](auto entityID)
			{
				Core::Entity e(entityID, m_scene);
				SerializeEntity(out, e);
			});
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		throw Proximity::Execptions::MethodNotImplemented("Deserializer not implemented");
		return false;
	}
}