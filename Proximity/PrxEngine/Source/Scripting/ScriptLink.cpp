#include "enginepch.h"
#include "Scripting/ScriptLink.h"
#include "Engine/Game/Entity.h"
#include "Physics/RaycastCallback.h"


namespace Proximity::Scripting
{
	ScriptLink::ScriptLink() = default;

	ScriptLink::ScriptLink(const std::string_view& scriptFilePath)
		:
		m_linkedEntity(nullptr),
		m_script(scriptFilePath.data())
	{}

	ScriptLink::~ScriptLink()
	{
		m_script.m_luaState.collect_garbage();
		SAFE_DELETE(m_linkedEntity)
	}

	const LuaScript& ScriptLink::GetLuaScript() const noexcept
	{
		return m_script;
	}

	bool ScriptLink::Compile()
	{
		return m_script.Compile();

	}
	void ScriptLink::LinkEntity(const Core::Entity& e)
	{
		m_linkedEntity = new Core::Entity(e);

		m_entityTable = m_script.m_luaState.create_table();

#pragma region Transform Data
		m_entityTable.set_function(
			"GetTransform", [this]()
			{
				return m_linkedEntity->GetComponent<Core::TransformComponent>();
			});
		m_entityTable.set_function(
			"SetTransform", [this](const Core::TransformComponent& tf)
			{
				auto& comp = this->m_linkedEntity->GetComponent<Core::TransformComponent>();
				comp.SetPosition(tf.m_Position);
				comp.SetRotation(tf.m_Rotation);
				comp.SetScale(tf.m_Scale);
			});

		m_entityTable.set_function(
			"GetPosition", [this]()
			{
				return m_linkedEntity->GetComponent<Core::TransformComponent>().m_Position;
			});
		m_entityTable.set_function(
			"SetPosition", [this](const Math::Vector3& pos)
			{
				auto& comp = this->m_linkedEntity->GetComponent<Core::TransformComponent>();
				comp.SetPosition(pos);
			});

		m_entityTable.set_function(
			"GetRotation", [this]()
			{
				return m_linkedEntity->GetComponent<Core::TransformComponent>().m_Rotation;
			});
		m_entityTable.set_function(
			"SetRotation", [this](const Math::Vector3& rot)
			{
				auto& comp = this->m_linkedEntity->GetComponent<Core::TransformComponent>();
				comp.SetRotation(rot);
			});

		m_entityTable.set_function(
			"GetScale", [this]()
			{
				return m_linkedEntity->GetComponent<Core::TransformComponent>().m_Scale;
			});
		m_entityTable.set_function(
			"SetScale", [this](const Math::Vector3& scale)
			{
				auto& comp = this->m_linkedEntity->GetComponent<Core::TransformComponent>();
				comp.SetScale(scale);
			});
#pragma endregion


#pragma region Physics Data
		m_entityTable.set_function(
			"DoRaycast", [this](Math::Vector3 start, Math::Vector3 end) -> Physics::RaycastManifold
			{
				Physics::RaycastCallback callback;
				b2Vec2 b2Start(start.x, start.y);
				b2Vec2 b2End(end.x, end.y);

				m_linkedEntity->GetScene()->m_physicsWorld->RayCast(&callback, b2Start, b2End);

				// Build manifold

				bool hit = (callback.m_fixture != nullptr) ? true : false;

				if (hit)
				{
					return Physics::RaycastManifold(
						hit,
						static_cast<Core::NameComponent*>(callback.m_fixture->GetBody()->GetUserData())->m_EntityName,
						callback.m_fraction,
						Math::Vector3(callback.m_normal.x, callback.m_normal.y, 0.0f),
						Math::Vector3(callback.m_point.x, callback.m_point.y, 0.0f));
				}
				else
				{
					return Physics::RaycastManifold(
						hit,
						"nil",
						callback.m_fraction,
						Math::Vector3(callback.m_normal.x, callback.m_normal.y, 0.0f),
						Math::Vector3(callback.m_point.x, callback.m_point.y, 0.0f));
				}
			}
		);
#pragma endregion

		m_script.m_luaState["_Entity"] = m_entityTable;
	}

	sol::object	ScriptLink::GetEntity()
	{
		return (m_linkedEntity) ? m_entityTable : sol::nil;
	}

	void ScriptLink::UnlinkEntity()
	{
		SAFE_DELETE(m_linkedEntity)
		
		m_entityTable = sol::nil;
	}

	void ScriptLink::CallOnStart()
	{
		m_script.OnStart();
	}

	void ScriptLink::CallOnUpdate(float dt)
	{
		m_script.OnUpdate(dt);
	}
	void ScriptLink::CallOnCollisionStart(const Physics::CollisionManifold& hit)
	{
		m_script.OnCollisionStart(hit);
	}
	void ScriptLink::CallOnCollisionEnd(const Physics::CollisionManifold& hit)
	{
		m_script.OnCollisionEnd(hit);
	}
}