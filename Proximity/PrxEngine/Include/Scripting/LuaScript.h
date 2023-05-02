#pragma once
#include "sol/sol.hpp"

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
		void OnCollisionStart(const std::string hitName);
		void OnCollisionEnd(const std::string hitName);

	private:
		void LogMsgToEditor(sol::object obj) const noexcept;
		void LogVec3ToEditor(sol::object obj) const noexcept;
		void LogTransformToEditor(sol::object obj) const noexcept;
		void CreateState();
		void CaptureFunctions();

	private:
		sol::state  m_luaState;
		std::string m_name;
		std::string m_filepath;

		sol::function m_OnStart;
		sol::function m_OnUpdate;
		sol::function m_OnCollisionStart;
		sol::function m_OnCollisionEnd;
	};
}