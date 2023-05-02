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
	public:
		ScriptLink();
		explicit ScriptLink(const std::string_view& scriptFilePath);
		~ScriptLink();

		const LuaScript& GetLuaScript() const noexcept;
				
		void LinkEntity(const Core::Entity& e);
		void UnlinkEntity();
		
		bool Compile();
		void CallOnStart();
		void CallOnUpdate(float dt);
		void CallOnCollisionStart(const std::string hitName);
		void CallOnCollisionEnd(const std::string hitName);

	private:
		sol::object GetEntity();

	private:
		Core::Entity* m_linkedEntity;
		sol::table    m_entityTable;
		LuaScript     m_script;
	};
}