#pragma once
#include "sol/sol.hpp"


namespace Proximity::Core
{
	class Entity;
}

namespace Proximity::Scripting
{
	class ScriptLink
	{
	public:
		ScriptLink() = default;
		ScriptLink(const std::string_view& scriptFilePath);
		~ScriptLink();

		inline const std::string& GetName() const noexcept { return m_name; }
		inline const std::string& GetPath() const noexcept { return m_scriptFilePath; }
		
		void LinkEntity(Core::Entity& e);
		void UnlinkEntity();
		
		bool Compile();

		void CallOnStart();
		void CallOnUpdate(float dt);
		void CallOnEnd();

	private:
		void DeclareVariable(const sol::object& name, const std::string type, sol::object value);
		void LogToEditor(std::string msg);
		sol::object LuaGetEntity();

	private:
		bool          m_hasEntity = false;
		std::string   m_name;
		std::string   m_scriptFilePath;
		sol::state    m_luaState;
		sol::function m_OnCompile;
		sol::function m_OnStart;
		sol::function m_OnUpdate;
		sol::function m_OnKeyboardInput;
		sol::function m_OnMouseInput;
		sol::function m_OnEnd;

		sol::table m_entity;
	};
}