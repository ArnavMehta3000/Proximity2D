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

	private:
		void LogToEditor(sol::object msg) const noexcept;
		void CreateState();
		void CaptureFunctions();

	private:
		sol::state  m_luaState;
		std::string m_name;
		std::string m_filepath;

		sol::function m_OnStart;
		sol::function m_OnUpdate;
	};
}