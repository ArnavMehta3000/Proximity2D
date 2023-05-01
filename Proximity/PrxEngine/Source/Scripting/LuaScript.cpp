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
		ScriptLink::CreateState(m_luaState);
	}

	LuaScript::~LuaScript() = default;

	bool LuaScript::Compile()
	{
		m_luaState.collect_garbage();
		if (auto result = m_luaState.safe_script_file(m_filepath); !result.valid())
		{
			sol::error e = result;
			PRX_LOG_ERROR("[SCRIPT ERROR] - %s", e.what());
			return false;
		}

		CaptureFunctions();

		return true;
	}

	void LuaScript::OnStart()
	{
		if (!m_OnStart.valid())
			return;

		m_OnStart.call();
	}

	void LuaScript::OnUpdate(float dt)
	{
		if (!m_OnUpdate.valid())
			return;

		m_OnUpdate.call(dt);
	}

	void LuaScript::CaptureFunctions()
	{
		// Link functions
		m_OnCompile = m_luaState["PRX"]["OnCompile"];
		m_OnStart   = m_luaState["PRX"]["OnStart"];
		m_OnUpdate  = m_luaState["PRX"]["OnUpdate"];

		if (m_OnCompile.valid())
			m_OnCompile.call();

	}
}