#include "enginepch.h"
#include "Scripting/ScriptLink.h"
#include "Engine/Game/Entity.h"
#include <regex>


namespace Proximity::Scripting
{
	ScriptLink::ScriptLink() = default;

	ScriptLink::ScriptLink(const std::string_view& scriptFilePath)
		:
		m_script(scriptFilePath.data()),
		m_linkedEntity(nullptr)
	{}

	ScriptLink::~ScriptLink()
	{
		SAFE_DELETE(m_linkedEntity)
	}

	const LuaScript& ScriptLink::GetLuaScript() const noexcept
	{
		return m_script;
	}

	bool ScriptLink::Compile()
	{
		return m_script.Compile();
	}

	void ScriptLink::CreateState(sol::state& lua)
	{
		lua = sol::state();

		lua.open_libraries(
			sol::lib::base,
			sol::lib::coroutine,
			sol::lib::package,
			sol::lib::string,
			sol::lib::os,
			sol::lib::math,
			sol::lib::table,
			sol::lib::debug,
			sol::lib::utf8);

		// Setup include diretories
		std::filesystem::path path = Utils::DirectoryManager::s_appDirectories.ScriptsPath;
		path /= "?.lua";
		lua["package"]["path"] = path.string().c_str();


		// Set functions
		sol::table luaTable = lua.create_table();
		luaTable.set_function("Log", &ScriptLink::LogToEditor);

		lua["PRX"] = luaTable;
	}

	void ScriptLink::LinkEntity(const Core::Entity& e)
	{
		m_linkedEntity = new Core::Entity(e);
	}

	void ScriptLink::UnlinkEntity()
	{
		SAFE_DELETE(m_linkedEntity)
		
		m_entityTable = sol::nil;
	}
	
	void ScriptLink::LogToEditor(const std::string& msg)
	{
		Core::Globals::g_editorDebugBuffer->AddToStream("[LUA LOG] " + msg);
	}

	void ScriptLink::CallOnStart()
	{
		m_script.OnStart();
	}

	void ScriptLink::CallOnUpdate(float dt)
	{
		m_script.OnUpdate(dt);
	}
}