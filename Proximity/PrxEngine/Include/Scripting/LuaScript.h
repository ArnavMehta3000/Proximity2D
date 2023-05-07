#pragma once
#include "sol/sol.hpp"
#include "Physics/CollisionManifold.h"
#include "Input/InputSystem.h"

namespace Proximity::Scripting
{
	class LuaScript
	{
		friend class ScriptLink;
	public:
		LuaScript() = default;

		explicit LuaScript(const std::string& filepath);
		~LuaScript();

		const auto& GetName()     const noexcept { return m_name; }
		const auto& GetFilePath() const noexcept { return m_filepath; }
		auto&       GetState()    const noexcept { return m_luaState; }

		bool Compile();
		void OnStart();
		void OnUpdate(float dt);
		void OnCollisionStart(const Physics::CollisionManifold& hit);
		void OnCollisionEnd(const Physics::CollisionManifold& hit);

		void OnKeyDown(const std::string& name);
		void OnKeyUp(const std::string& name);
		void OnMouseLBDown();
		void OnMouseRBDown();
		void OnMouseMBDown();
		void OnMouseLBUp();
		void OnMouseRBUp();
		void OnMouseMBUp();

	private:
		void LogMsgToEditor(sol::object obj) const noexcept;
		void LogVec3ToEditor(sol::object obj) const noexcept;
		void LogTransformToEditor(sol::object obj) const noexcept;
		void CreateState();
		void CaptureFunctions();

		bool HasKeyboardCapture() const noexcept { return m_OnKeyDown.valid() || m_OnKeyUp.valid(); }
		bool HasMouseCapture() const noexcept { return m_OnMouseInput.valid(); }

	private:
		sol::state  m_luaState;
		std::string m_name;
		std::string m_filepath;

		sol::function m_OnStart;
		sol::function m_OnUpdate;
		sol::function m_OnCollisionStart;
		sol::function m_OnCollisionEnd;
		sol::function m_OnKeyDown;
		sol::function m_OnKeyUp;
		sol::function m_OnMouseInput;
	};
}