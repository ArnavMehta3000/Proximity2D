#pragma once
#include "Engine/Components/BaseComponent.h"
#include "Graphics/Material.h"
#include "Graphics/VertexStructures.h"
#include "../External/entt/entt.hpp"
#include "Engine/Audio/SoundSystem.h"
#include "Scripting/ScriptLink.h"

namespace Proximity::Core
{
	struct NameComponent : public BaseComponent
	{
		NameComponent();
		NameComponent(const std::string& name);
		NameComponent(const NameComponent&) = default;

		std::string m_EntityName;
	};




	struct TransformComponent : public BaseComponent
	{
		TransformComponent();
		TransformComponent(Math::Vec3 pos);
		TransformComponent(Math::Vec3 pos, Math::Vector3 rot);
		TransformComponent(Math::Vec3 pos, Math::Vector3 rot, Math::Vec3 scale);
		TransformComponent(const TransformComponent&) = default;
		
		Math::Vector3 m_Position;
		Math::Vector3 m_Rotation;
		Math::Vector3 m_Scale;
		entt::entity  m_parentHandle;

		Math::Matrix GetWorldMatrix() const noexcept;

		operator Math::Matrix () noexcept
		{
			return DX::XMMatrixScalingFromVector(m_Scale) * DX::XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z) * DX::XMMatrixTranslationFromVector(m_Position);
		}

		operator const Math::Matrix() noexcept
		{
			return DX::XMMatrixScalingFromVector(m_Scale) * DX::XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z) * DX::XMMatrixTranslationFromVector(m_Position);
		}
	};

	

	struct SpriteRendererComponent : public BaseComponent
	{
		SpriteRendererComponent();
		std::shared_ptr<Graphics::Material> m_Material;
	};



	struct AudioSourceComponent : public BaseComponent
	{
		AudioSourceComponent();
		std::shared_ptr<Audio::AudioSource> m_Source;
	};

	
	class ScriptableEntity;

	struct InternalScriptComponent : public BaseComponent
	{
		InternalScriptComponent()
			:
			BaseComponent("Internal Script")
		{}


		Core::ScriptableEntity* m_Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void(*DestroyInstanceFunction)(InternalScriptComponent*);

		template <typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<Core::ScriptableEntity*>(new T()); };
			DestroyInstanceFunction = [](InternalScriptComponent* script) { delete script->m_Instance; script->m_Instance = nullptr; };
		}
	};


	struct LuaScriptComponent : public BaseComponent
	{
		LuaScriptComponent()
			:
			BaseComponent("Lua Script Component")
		{}

		Scripting::ScriptLink m_ScriptLink;
	};


	// Physics
	struct RigidBody2DComponent : public BaseComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		
		RigidBody2DComponent();
		RigidBody2DComponent(const RigidBody2DComponent&) = default;
		
		static std::string BodyTypeToString(Core::RigidBody2DComponent::BodyType bodyType);
		static RigidBody2DComponent::BodyType StringToBodyType(const std::string& bodyType);

		BodyType m_Type;
		bool  m_FixedRotation;
		void* m_runtimeBody;
	};


	struct BoxCollider2DComponent : public BaseComponent
	{
		BoxCollider2DComponent();
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;

		Math::F32 m_Offset[2];
		Math::F32 m_Size[2];

		Math::F32 m_Density;
		Math::F32 m_Friction;
		Math::F32 m_Restitution;

		void* m_runtimeFixture;
	};
}