#include "enginepch.h"
#include "Scripting/ScriptLink.h"
#include "sol/sol.hpp"


namespace Proximity::Scripting
{
	struct ScriptLinkData
	{
		sol::state LuaState;
	};

	static ScriptLinkData* s_data;

	const ScriptLinkData* ScriptLink::GetData() { return s_data; }


	void ScriptLink::Init()
	{
		s_data = new ScriptLinkData;
		s_data->LuaState.open_libraries(
			sol::lib::base,
			sol::lib::package,
			sol::lib::coroutine,
			sol::lib::string,
			sol::lib::os,
			sol::lib::math,
			sol::lib::table,
			sol::lib::debug,
			sol::lib::utf8);

		int x = 0;
	}

	void ScriptLink::Shutdown()
	{
		

		delete s_data;
		s_data = nullptr;
	}
}