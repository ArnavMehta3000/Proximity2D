#pragma once

namespace sol
{
	class state;
}

namespace Proximity::Scripting
{
	class ScriptLink
	{
	public:

		// Create a lua state object linked with a file with all the information from C++
		static sol::state CreateLuaState(std::string luaFilepath = "");
	};
}