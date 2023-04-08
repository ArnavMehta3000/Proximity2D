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
		out << YAML::Key << "Component Base Data" << YAML::Value;
		out << YAML::BeginMap;
			out << YAML::Key << "View Name" << YAML::Value << base.m_ViewName;
			out << YAML::Key << "Component ID" << YAML::Value << base.m_ComponentID;
		out << YAML::EndMap;
	}

	static void SerializeEntity(YAML::Emitter& out, Core::Entity entity)
	{
		out << YAML::Key << "Entity" << YAML::Value << "ADD UUID";

		// Serialize name component
		{
			auto& nameComp = entity.GetComponent<Core::NameComponent>();

			out << YAML::Key << "Name Component" << YAML::Value;
			out << YAML::BeginMap;
				SerializeBaseComponent(out, nameComp);
				out << YAML::Key << "Data" << YAML::Value;
				out << YAML::BeginMap;
					out << YAML::Key << "EntityName" << YAML::Value << nameComp.m_EntityName;
				out << YAML::EndMap;
			out << YAML::EndMap;
		}

		// Serialize transform component
		{
			auto& transfromComp = entity.GetComponent<Core::TransformComponent>();

			out << YAML::Key << "Transform Component" << YAML::Value;
			out << YAML::BeginMap;
				SerializeBaseComponent(out, transfromComp);
				out << YAML::Key << "Data" << YAML::Value;
				out << YAML::Value;
				out << YAML::BeginMap;
					out << YAML::Key << "Position" << YAML::Value << transfromComp.m_Position;
					out << YAML::Key << "Rotation" << YAML::Value << transfromComp.m_Rotation;
					out << YAML::Key << "Scale" << YAML::Value << transfromComp.m_Scale;
				out << YAML::EndMap;
			out << YAML::EndMap;
		}

		// Serialize sprite renderer component
		{
			auto& sr = entity.GetComponent<Core::SpriteRendererComponent>();

			out << YAML::Key << "Sprite Renderer Component" << YAML::Value;
			out << YAML::BeginMap;
				SerializeBaseComponent(out, sr);
				out << YAML::Key << "Data" << YAML::Value;
				out << YAML::BeginMap;
				out << "Material Name" << YAML::Value << ((sr.Material == nullptr) ? "null" : sr.Material->GetName());
				out << YAML::EndMap;
			out << YAML::EndMap;
		}
	}
	
	bool SceneSerializer::Serialize()
	{
		if (m_scene == nullptr)
			return false;

		YAML::Emitter out;
		
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << m_scene->GetName();
		m_scene->m_sceneRegistry.each([&](auto entityID)
			{
				Core::Entity e(entityID, m_scene);
				SerializeEntity(out, e);
			});
		out << YAML::EndMap;

		Utils::FilePath path = Utils::DirectoryManager::s_appDirectories.ScenesPath;
		std::ofstream fout(path / (m_scene->GetName() + "prxscene"));
		fout << out.c_str();
		fout.close();
		return true;
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		throw Proximity::Execptions::MethodNotImplemented("Deserializer not implemented");
		return false;
	}
}