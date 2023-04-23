#include "enginepch.h"
#include "Scripting/ScriptLink.h"

extern "C"
{
	#include <lua/include/lua.h>
	#include <lua/include/lauxlib.h>
	#include <lua/include/lualib.h>
}

namespace Proximity::Scripting
{
	void ScriptLink::Init()
	{
		std::string cmd = "a = 7 + 11";

		lua_State* L = luaL_newstate();

		int r = luaL_dostring(L, cmd.c_str());
		luaL_openlibs(L);
		if (r == LUA_OK)
		{
			lua_getglobal(L, "a");
			if (lua_isnumber(L, -1))
			{
				float a = (float)lua_tonumber(L, -1);
				PRX_LOG_DEBUG("LUA EXEC: %f", a);
			}
		}
		else
		{
			std::string errorMsg = lua_tostring(L, -1);
			PRX_LOG_ERROR("LUA ERROR: %s", errorMsg.c_str());
		}

		lua_close(L);
	}

	void ScriptLink::Shutdown()
	{
	}
}