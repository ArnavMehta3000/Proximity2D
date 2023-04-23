#include "enginepch.h"
#include "Scripting/ScriptLink.h"
#include "sol/sol.hpp"
#include "Engine/Game/Entity.h"


namespace Proximity::Scripting
{
	sol::state ScriptLink::CreateLuaState(std::string luaFilepath)
	{
		sol::state lua;
		lua.open_libraries(
			sol::lib::base,
			sol::lib::package,
			sol::lib::coroutine,
			sol::lib::string,
			sol::lib::os,
			sol::lib::math,
			sol::lib::table,
			sol::lib::debug,
			sol::lib::utf8);

		// TODO: Use directory manager to add scripts path to package path (? loads all lua files)
		std::filesystem::path path = Utils::DirectoryManager::s_appDirectories.ScriptsPath;
		path /= "?.lua" ;
		lua["package"]["path"] = path.string().c_str();

		lua.script_file(luaFilepath);
		lua["CallInclude"].call();
		return lua;
	}
}