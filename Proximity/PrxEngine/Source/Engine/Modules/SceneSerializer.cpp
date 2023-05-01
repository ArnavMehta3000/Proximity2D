#include "enginepch.h"
#include <yaml-cpp/yaml.h>
#include "Engine/Components/Components.h"
#include "Engine/Modules/SceneSerializer.h"
#include "Engine/Modules/MaterialLibrary.h"
#include "Engine/Modules/AudioLibrary.h"
#include "Engine/Modules/ScriptLibrary.h"
#include "Utils/DirectoryManager.h"
#include "Utils/Exceptions.h"

namespace YAML
{
	template<>
	struct convert<Proximity::Math::Vector3>
	{
		static Node encode(const Proximity::Math::Vector3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, Proximity::Math::Vector3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();

			return true;
		}
	};
}


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

	static void SerializeEntity(YAML::Emitter& out, Core::Entity entity)
	{
		auto& nameComp = entity.GetComponent<Core::NameComponent>();

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << nameComp.m_EntityName;
		

		// Serialize transform component
		{
			auto& tc = entity.GetComponent<Core::TransformComponent>();

			out << YAML::Key << "Transform Component";
			out << YAML::BeginMap;
				out << YAML::Key << "Position" << YAML::Value << tc.m_Position;
				out << YAML::Key << "Rotation" << YAML::Value << tc.m_Rotation;
				out << YAML::Key << "Scale" << YAML::Value << tc.m_Scale;
			out << YAML::EndMap;
		}

		// Serialize sprite renderer component
		{
			auto& sr = entity.GetComponent<Core::SpriteRendererComponent>();

			out << YAML::Key << "Sprite Renderer Component";
			out << YAML::BeginMap;
				out << YAML::Key << "Material Name" << YAML::Value << ((sr.m_Material == nullptr) ? "null" : sr.m_Material->GetName());
			out << YAML::EndMap;
		}

		// Serialize rigid body component
		if (entity.HasComponent<Core::RigidBody2DComponent>())
		{
			auto& rb = entity.GetComponent<Core::RigidBody2DComponent>();
			
			out << YAML::Key << "Rigid Body 2D Component";
			out << YAML::BeginMap;
				out << YAML::Key << "Body Type" << YAML::Value << Core::RigidBody2DComponent::BodyTypeToString(rb.m_Type);
				out << YAML::Key << "Fixed Rotation" << YAML::Value << rb.m_FixedRotation;
			out << YAML::EndMap;
		}

		// Serialize box collider component
		if (entity.HasComponent<Core::BoxCollider2DComponent>())
		{
			auto& collider = entity.GetComponent<Core::BoxCollider2DComponent>();

			out << YAML::Key << "Box Collider 2D Component";
			out << YAML::BeginMap;
				out << YAML::Key << "Offset" << YAML::Value << Vector3(collider.m_Offset[0], collider.m_Offset[1], 0.0f);
				out << YAML::Key << "Size" << YAML::Value << Vector3(collider.m_Size[0], collider.m_Size[1], 0.0f);
				out << YAML::Key << "Density" << YAML::Value << collider.m_Density;
				out << YAML::Key << "Friction" << YAML::Value << collider.m_Density;
				out << YAML::Key << "Restitution" << YAML::Value << collider.m_Restitution;
			out << YAML::EndMap;
		}

		// Serialiaze audio component
		if (entity.HasComponent<Core::AudioSourceComponent>())
		{
			auto& src = entity.GetComponent<Core::AudioSourceComponent>();

			out << YAML::Key << "Audio Source Component";
			out << YAML::BeginMap;
				out << YAML::Key << "Source Name" << YAML::Value << ((src.m_Source == nullptr) ? "null" : src.m_Source->Name);
			out << YAML::EndMap;
		}


		// Serialize lua script component
		if (entity.HasComponent<Core::LuaScriptComponent>())
		{
			auto& lua = entity.GetComponent<Core::LuaScriptComponent>();

			out << YAML::Key << "Lua Script Component";
			out << YAML::BeginMap;
				out << YAML::Key << "Script Name" << YAML::Value << ((lua.m_Link == nullptr) ? "null" : lua.m_Link->GetLuaScript().GetName());
			out << YAML::EndMap;
		}


		out << YAML::EndMap;
	}
	
	bool SceneSerializer::Serialize()
	{
		if (m_scene == nullptr)
			return false;

		YAML::Emitter out;
		
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_scene->m_sceneRegistry.each([&](auto entityID)
			{
				Core::Entity entity = { entityID, m_scene };
				SerializeEntity(out, entity);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(m_scene->GetFilePath());
		
		fout << out.c_str();
		fout.close();
		
		return true;
	}






	Core::Scene* SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data = YAML::LoadFile(filepath);
		if (!data["Scene"])
			return nullptr;

		auto sceneName = data["Scene"].as<std::string>();

		if (m_scene == nullptr)
			m_scene = new Core::Scene(sceneName, filepath);

		// Check for entities
		YAML::Node entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				Core::Entity deserializedEntity = Core::Entity(m_scene->m_sceneRegistry.create(), m_scene);

				auto name = entity["Entity"].as<std::string>();

				// Add name component
				deserializedEntity.AddComponent<Core::NameComponent>(name);

				// Add transform component
				auto transform = entity["Transform Component"];
				if (transform)
				{
					auto& tc = deserializedEntity.AddComponent<Core::TransformComponent>();

					tc.m_Position = transform["Position"].as<Math::Vector3>();
					tc.m_Rotation = transform["Rotation"].as<Math::Vector3>();
					tc.m_Scale    = transform["Scale"].as<Math::Vector3>();
				}

				auto spriteRenderer = entity["Sprite Renderer Component"];
				if (spriteRenderer)
				{
					auto& sr = deserializedEntity.AddComponent<Core::SpriteRendererComponent>();
					
					std::string matName = spriteRenderer["Material Name"].as<std::string>();
					if (matName == "null")
					{
						sr.m_Material = nullptr;
					}
					else
					{
						// Try get the material from the library
						auto mat = PRX_RESOLVE(Modules::MaterialLibrary)->Get(matName);
						if (mat != nullptr)
							sr.m_Material = mat;
						else
							sr.m_Material = nullptr;
					}
				}

				auto rigidBody2D = entity["Rigid Body 2D Component"];
				if (rigidBody2D)
				{
					auto& rb = deserializedEntity.AddComponent<Core::RigidBody2DComponent>();

					rb.m_Type          = Core::RigidBody2DComponent::StringToBodyType(rigidBody2D["Body Type"].as<std::string>());
					rb.m_FixedRotation = rigidBody2D["Fixed Rotation"].as<bool>();
				}

				auto boxCol = entity["Box Collider 2D Component"];
				if (boxCol)
				{
					auto& col = deserializedEntity.AddComponent<Core::BoxCollider2DComponent>();

					auto vecOffset = boxCol["Offset"].as<Math::Vector3>();
					auto vecSize   = boxCol["Size"].as<Math::Vector3>();
					
					col.m_Offset[0]   = vecOffset.x;
					col.m_Offset[1]   = vecOffset.y;
					col.m_Size[0]     = vecSize.x;
					col.m_Size[1]     = vecSize.y;
					col.m_Density     = boxCol["Density"].as<float>();
					col.m_Friction    = boxCol["Friction"].as<float>();
					col.m_Restitution = boxCol["Restitution"].as<float>();
				}

				auto audioSource = entity["Audio Source Component"];
				if (audioSource)
				{
					auto& src = deserializedEntity.AddComponent<Core::AudioSourceComponent>();

					std::string srcName = audioSource["Source Name"].as<std::string>();
					if (srcName == "null")
					{
						src.m_Source = nullptr;
					}
					else
					{
						auto audio = PRX_RESOLVE(Modules::AudioLibrary)->Get(srcName);
						if (audio != nullptr)
							src.m_Source = audio;
						else
							src.m_Source = nullptr;
					}
				}
				auto lua = entity["Lua Script Component"];
				if (lua)
				{
					auto& comp = deserializedEntity.AddComponent<Core::LuaScriptComponent>();

					std::string scriptName = lua["Script Name"].as<std::string>();
					if (scriptName == "null")
					{
						comp.m_Link = nullptr;
					}
					else
					{
						auto luaScript = PRX_RESOLVE(Modules::ScriptLibrary)->Get(scriptName);
						if (luaScript != nullptr)
						{
							comp.m_Link = std::make_unique<Scripting::ScriptLink>(luaScript->GetFilePath());;
							comp.m_Link->LinkEntity(deserializedEntity);
						}
						else
							comp.m_Link = nullptr;
					}
				}
			}
		}
		return m_scene;
	}
}