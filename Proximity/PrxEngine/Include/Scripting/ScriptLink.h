#pragma once
#include "Scripting/LuaScript.h"


namespace Proximity::Core
{
	class Entity;
}

namespace Proximity::Scripting
{
	class ScriptLink
	{
		friend class LuaScript;

	public:
		ScriptLink();
		explicit ScriptLink(const std::string_view& scriptFilePath);
		~ScriptLink();

		const LuaScript& GetLuaScript() const noexcept;

		static void CreateState(sol::state& lua);
		static void LogToEditor(const std::string& msg);
		
		void LinkEntity(const Core::Entity& e);
		void UnlinkEntity();
		
		bool Compile();
		void CallOnStart();
		void CallOnUpdate(float dt);

	private:
		Core::Entity* m_linkedEntity;
		sol::table    m_entityTable;
		LuaScript     m_script;
	};
}