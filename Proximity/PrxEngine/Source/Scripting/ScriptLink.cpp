#include "enginepch.h"
#include "Scripting/ScriptLink.h"
#include "Engine/Game/Entity.h"
#include <regex>


namespace Proximity::Scripting
{
	ScriptLink::ScriptLink(const std::string_view& scriptFilePath)
		:
		m_hasEntity(false)
	{
		m_name           = Utils::DirectoryManager::GetFileNameFromDir(scriptFilePath.data(), true);
		m_scriptFilePath = scriptFilePath.data();
		
		m_luaState = sol::state();

		m_luaState.open_libraries(
			sol::lib::base,
			sol::lib::package,
			sol::lib::coroutine,
			sol::lib::string,
			sol::lib::os,
			sol::lib::math,
			sol::lib::table,
			sol::lib::debug,
			sol::lib::utf8);

		std::filesystem::path path = Utils::DirectoryManager::s_appDirectories.ScriptsPath;
		path /= "?.lua";
		m_luaState["package"]["path"] = path.string().c_str();



		// Set functions
		sol::table luaTable = m_luaState.create_table();
		luaTable.set_function("Log", &ScriptLink::LogToEditor, this);
		luaTable.set_function("Declare", &ScriptLink::DeclareVariable, this);
		luaTable.set_function("GetEntity", &ScriptLink::LuaGetEntity, this);

		m_luaState["Proximity"] = luaTable;
	}

	ScriptLink::~ScriptLink()
	{
	}

	bool ScriptLink::Compile()
	{
		auto result = m_luaState.load_file(m_scriptFilePath);
		if (!result.valid())
		{
			sol::error e = result;
			PRX_LOG_ERROR("[SCRIPT ERROR] - %s", e.what());
			return false;
		}
		PRX_LOG_INFO("LUA script [%s] compiled", m_name.c_str());
		result();

		// Link functions
		m_OnCompile       = m_luaState["Proximity"]["OnCompile"];
		m_OnStart         = m_luaState["Proximity"]["OnStart"];
		m_OnUpdate        = m_luaState["Proximity"]["OnUpdate"];
		m_OnKeyboardInput = m_luaState["Proximity"]["OnKeyboardInput"];
		m_OnMouseInput    = m_luaState["Proximity"]["OnMouseInput"];
		m_OnEnd           = m_luaState["Proximity"]["OnEnd"];

		if (m_OnCompile.valid())
			m_OnCompile.call();

		return true;
	}

	void ScriptLink::DeclareVariable(const sol::object& name, const std::string type, sol::object value)
	{
		if (type == "string")
			m_luaState[name] = value.as<std::string>();
		else if (type == "int")
			m_luaState[name] = value.as<int>();
		else if (type == "float")
			m_luaState[name] = value.as<float>();
		else if (type == "bool")
			m_luaState[name] = value.as<bool>();
		else
		{
			PRX_LOG_ERROR("LUA script variable type [%s] not supported", type.c_str());
		}
	}



	inline void ScriptLink::LinkEntity(Core::Entity& e)
	{
		sol::table transform  = m_luaState.create_table();
		transform["position"] = e.GetComponent<Proximity::Core::TransformComponent>().m_Position;
		transform["rotation"] = e.GetComponent<Proximity::Core::TransformComponent>().m_Rotation;
		transform["scale"]    = e.GetComponent<Proximity::Core::TransformComponent>().m_Scale;

		// Try get other components

		// Create a Lua table to hold the entity data
		m_entity              = m_luaState.create_table();
		m_entity["name"]      = e.GetComponent<Proximity::Core::NameComponent>().m_EntityName;
		m_entity["transform"] = transform;

		m_hasEntity = true;
	}
	
	void ScriptLink::LogToEditor(std::string msg)
	{
		Core::Globals::g_editorDebugBuffer->AddToStream("[LUA LOG] " + msg);
	}

	sol::object ScriptLink::LuaGetEntity()
	{
		return (m_hasEntity) ? m_entity : sol::nil;
	}

	void ScriptLink::CallOnStart()
	{
	}

	void ScriptLink::CallOnUpdate(float dt)
	{
	}

	void ScriptLink::CallOnEnd()
	{
	}

}