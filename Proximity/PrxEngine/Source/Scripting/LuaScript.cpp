#include "enginepch.h"
#include "Scripting/LuaScript.h"
#include "Scripting/ScriptLink.h"
#include "Engine/Game/Entity.h"

namespace Proximity::Scripting
{
	LuaScript::LuaScript(const std::string& filepath)
		:
		m_filepath(filepath)
	{
		m_name = Utils::DirectoryManager::GetFileNameFromDir(filepath, true);
		CreateState();
	}

	LuaScript::~LuaScript()
	{
		m_luaState.collect_garbage();
	}

	void LuaScript::CreateState()
	{
		m_luaState = sol::state();

		using enum sol::lib;
		m_luaState.open_libraries(
			base,   coroutine, package,
			string, os,        math,
			table,  debug,     utf8);

		// Setup include diretories
		std::filesystem::path path = Utils::DirectoryManager::s_appDirectories.ScriptsPath;
		path /= "?.lua";
		m_luaState["package"]["path"] = path.string().c_str();

		// Set functions
		sol::table luaTable = m_luaState.create_table();
		luaTable.set_function("Log", &LuaScript::LogToEditor, this);
		m_luaState["PRX"] = luaTable;


		// Register types
		auto vec3Type = m_luaState.new_usertype<Math::Vector3>(
			"Vector3",
			sol::constructors<Math::Vector3(), Math::Vector3(float, float, float), Math::Vector3(float)>(),
			sol::base_classes, sol::bases<DirectX::XMFLOAT3>());
		vec3Type["x"] = &DirectX::XMFLOAT3::x;
		vec3Type["y"] = &DirectX::XMFLOAT3::y;
		vec3Type["z"] = &DirectX::XMFLOAT3::z;

		auto transformType = m_luaState.new_usertype<Core::TransformComponent>(
			"Transform",
			sol::constructors<
				Core::TransformComponent(), 
				Core::TransformComponent(Math::Vector3),
				Core::TransformComponent(Math::Vector3, Math::Vector3),
				Core::TransformComponent(Math::Vector3, Math::Vector3, Math::Vector3),
				Core::TransformComponent(Core::TransformComponent)>());
		transformType["position"] = &Core::TransformComponent::m_Position;
		transformType["rotation"] = &Core::TransformComponent::m_Rotation;
		transformType["scale"]    = &Core::TransformComponent::m_Scale;
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

	void LuaScript::LogToEditor(sol::object msg) const noexcept
	{
		auto str = msg.as<std::string>();
		Core::Globals::g_editorDebugBuffer->AddToStream(str);
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

		m_OnUpdate(dt);
	}

	void LuaScript::CaptureFunctions()
	{
		// Link functions
		m_OnStart   = m_luaState["PRX"]["OnStart"];
		m_OnUpdate  = m_luaState["PRX"]["OnUpdate"];


	}
}