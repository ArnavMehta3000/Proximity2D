#include "enginepch.h"
#include "Scripting/LuaScript.h"
#include "Scripting/ScriptLink.h"

namespace Proximity::Scripting
{
	LuaScript::LuaScript(const std::string& filepath)
		:
		m_filepath(filepath)
	{
		m_name = Utils::DirectoryManager::GetFileNameFromDir(filepath, true);
		CreateState();
	}

	LuaScript::~LuaScript() = default;

	void LuaScript::CreateState()
	{
		m_luaState = sol::state();

		m_luaState.open_libraries(
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
		m_luaState["package"]["path"] = path.string().c_str();

		// Set functions
		sol::table luaTable = m_luaState.create_table();
		luaTable.set_function("Log", &LuaScript::LogToEditor, this);

		m_luaState["PRX"] = luaTable;
	}

	bool LuaScript::Compile()
	{
		auto result = m_luaState.load_file(m_filepath);
		if (!result.valid())
		{
			sol::error e = result;
			PRX_LOG_ERROR("[SCRIPT ERROR] - %s", e.what());
			return false;
		}
		result();

		CaptureFunctions();

		return true;
	}

	void LuaScript::LogToEditor(const std::string& msg) const noexcept
	{
		Core::Globals::g_editorDebugBuffer->AddToStream("[LUA LOG] " + msg);
	}

	void LuaScript::OnStart()
	{
		if (!m_OnStart.valid())
			return;

		m_OnStart();
	}

	void LuaScript::OnUpdate(float dt)
	{
		if (!m_OnUpdate.valid())
			return;

		m_OnUpdate(dt);
	}

	void LuaScript::CaptureFunctions()
	{
		// Link functions
		m_OnStart   = m_luaState["PRX"]["OnStart"];
		m_OnUpdate  = m_luaState["PRX"]["OnUpdate"];


	}
}