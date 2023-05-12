#include "enginepch.h"
#include "Scripting/ScriptLink.h"
#include "Engine/Game/Entity.h"
#include "Physics/RaycastCallback.h"
#include "Input/InputSystem.h"


namespace Proximity::Scripting
{
	ScriptLink::ScriptLink() = default;

	ScriptLink::ScriptLink(const std::string_view& scriptFilePath)
		:
		m_linkedEntity(nullptr),
		m_script(scriptFilePath.data())
	{
	}

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

#pragma region Transform Functions
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

#pragma region Physics Functions
		m_entityTable.set_function(
			"DoRaycast", [this](Math::Vector3 start, Math::Vector3 end)
			{
				Physics::RaycastCallback callback;
				b2Vec2 b2Start(start.x, start.y);
				b2Vec2 b2End(end.x, end.y);

				m_linkedEntity->GetScene()->m_physicsWorld->RayCast(&callback, b2Start, b2End);

				// Build manifold

				bool hit = (callback.m_fixture != nullptr) ? true : false;

				if (hit)
				{
					auto distance = (b2End - b2Start).Length() * callback.m_fraction;

					return Physics::RaycastManifold(
						hit,
						static_cast<Core::NameComponent*>(callback.m_fixture->GetBody()->GetUserData())->m_EntityName,
						Math::Utils::Abs(distance),
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

		m_entityTable.set_function(
			"ApplyForce", [this](float x, float y)
			{
				if (!m_linkedEntity->HasComponent<Core::RigidBody2DComponent>())
				{
					PRX_LOG_WARN("Entity [%s] does not have rigid body 2d component and is trying to apply force!",
						m_linkedEntity->GetComponent<Core::NameComponent>().m_EntityName.c_str());
					return;
				}

				b2Vec2 f(x, y);
				const auto& rb = m_linkedEntity->GetComponent<Core::RigidBody2DComponent>();
				static_cast<b2Body*>(rb.m_runtimeBody)->ApplyForceToCenter(f, true);
			}
		);
#pragma endregion

#pragma region Audio Functions
		m_entityTable.set_function(
			"PlayAudio", [this](bool loop)
			{
				if (!m_linkedEntity->HasComponent<Core::AudioSourceComponent>())
				{
					PRX_LOG_WARN("Entity [%s] does not have audio source component and is trying to play audio!",
						m_linkedEntity->GetComponent<Core::NameComponent>().m_EntityName.c_str());
					return;
				}

				const auto& comp = m_linkedEntity->GetComponent<Core::AudioSourceComponent>();
				if (comp.m_Source == nullptr)
				{
					PRX_LOG_WARN("Entity [%s] does not have audio clip assigned and is trying to play audio!",
						m_linkedEntity->GetComponent<Core::NameComponent>().m_EntityName.c_str());
					return;
				}

				comp.m_Source->Play(loop);
			}
		);

		m_entityTable.set_function(
			"PlayAudioOneShot", [this]()
			{
				if (!m_linkedEntity->HasComponent<Core::AudioSourceComponent>())
				{
					PRX_LOG_ERROR("Entity [%s] does not have audio soiurce component and is trying to play audio",
						m_linkedEntity->GetComponent<Core::NameComponent>().m_EntityName.c_str());
					return;
				}

				const auto& comp = m_linkedEntity->GetComponent<Core::AudioSourceComponent>();
				comp.m_Source->PlayOneShot();
			}
		);
#pragma endregion


		m_script.m_luaState["_Entity"] = m_entityTable;
	}

	sol::object	ScriptLink::GetEntity() const noexcept
	{
		return m_linkedEntity ? m_entityTable : sol::nil;
	}

#pragma region Input Captures
	void ScriptLink::OnKeyboard(Core::Input::KeyInfo keyInfo)
	{
		if (Core::Globals::g_editorIsPlaying && Core::Globals::g_viewportIsFocused && m_script.HasKeyboardCapture())
		{
			auto name = Core::Input::KeyCodeToString(keyInfo.Key);
			if (name.empty())
				return;
			if (keyInfo.State.m_isUp && m_script.m_OnKeyUp.valid())
				m_inputQueue.push([=]() { m_script.m_OnKeyUp(name); });
			if (keyInfo.State.m_isDown && m_script.m_OnKeyDown.valid())
				m_inputQueue.push([=]() { m_script.m_OnKeyDown(name); });
		}
	}
#pragma endregion


	void ScriptLink::UnlinkEntity()
	{
		SAFE_DELETE(m_linkedEntity)

			m_entityTable = sol::nil;
	}

	void ScriptLink::EnableInput(bool enable)
	{
		if (enable)
		{
			Core::Input::OnKeyUp += PRX_ACTION_FUNC(ScriptLink::OnKeyboard);
			Core::Input::OnKeyDown += PRX_ACTION_FUNC(ScriptLink::OnKeyboard);
		}
		else
		{
			Core::Input::OnKeyUp -= PRX_ACTION_FUNC(ScriptLink::OnKeyboard);
			Core::Input::OnKeyDown -= PRX_ACTION_FUNC(ScriptLink::OnKeyboard);
		}
	}

	void ScriptLink::CallOnStart()
	{
		// Empty input queue (using the swapping with empty queue method)
		std::queue<std::function<void()>>().swap(m_inputQueue);

		m_script.OnStart();
	}

	void ScriptLink::CallOnUpdate(float dt)
	{
		// Create a temporary queue to parse input (to prevent queue size changing while executing
		std::queue<std::function<void()>> tempQueue;
		while (!m_inputQueue.empty())
		{
			auto& input = m_inputQueue.front();
			tempQueue.push(std::move(input));
			m_inputQueue.pop();
		}

		// Execute input calls in tempQueue
		while (!tempQueue.empty())
		{
			auto const& input = tempQueue.front();
			input();
			tempQueue.pop();
		}

		// Input Stack is now empty, so we can swap it with the temporary queue
		std::swap(m_inputQueue, tempQueue);

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