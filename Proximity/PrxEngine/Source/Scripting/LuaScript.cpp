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

	LuaScript::~LuaScript() = default;

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
		luaTable.set_function("Log", &LuaScript::LogMsgToEditor, this);
		luaTable.set_function("LogVec3", &LuaScript::LogVec3ToEditor, this);
		luaTable.set_function("LogTransform", &LuaScript::LogTransformToEditor, this);
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
				Core::TransformComponent(Core::TransformComponent)>(),
			sol::base_classes, sol::bases<Core::BaseComponent>());
		transformType["position"] = &Core::TransformComponent::m_Position;
		transformType["rotation"] = &Core::TransformComponent::m_Rotation;
		transformType["scale"]    = &Core::TransformComponent::m_Scale;

		auto colManifoldType = m_luaState.new_usertype<Physics::CollisionManifold>(
			"CollisionManifold",
			sol::constructors<Physics::CollisionManifold(std::string, Math::Vector3, Math::Vector3)>());
		colManifoldType["hitName"] = &Physics::CollisionManifold::m_HitName;
		colManifoldType["point"]   = &Physics::CollisionManifold::m_Point;
		colManifoldType["normal"]  = &Physics::CollisionManifold::m_Normal;
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

	void LuaScript::LogMsgToEditor(sol::object obj) const noexcept
	{
		auto str = obj.as<std::string>();
		Core::Globals::g_editorDebugBuffer->AddToStream(str);
	}
	void LuaScript::LogVec3ToEditor(sol::object obj) const noexcept
	{
		Math::Vector3 v = obj.as<Math::Vector3>();
		std::stringstream stream;

		stream << "Vector3[" << v.x << ", " << v.y << ", " << v.z << "]";

		auto str = stream.str();
		Core::Globals::g_editorDebugBuffer->AddToStream(str);
	}

	void LuaScript::LogTransformToEditor(sol::object obj) const noexcept
	{
		Core::TransformComponent t = obj.as<Core::TransformComponent>();
		std::stringstream stream;

		auto const& p = t.m_Position;
		auto const& r = t.m_Rotation;
		auto const& s = t.m_Scale;

		stream << "Position [" << p.x << ", " << p.y << ", " << p.z << "]" << "\t";
		stream << "Rotation [" << r.x << ", " << r.y << ", " << r.z << "]" << "\t";
		stream << "Scale [" << s.x << ", " << s.y << ", " << s.z << "]";

		auto str = stream.str();
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

	void LuaScript::OnCollisionStart(const Physics::CollisionManifold& hit)
	{
		if (!m_OnCollisionStart.valid())
			return;

		m_OnCollisionStart.call(hit);
	}

	void LuaScript::OnCollisionEnd(const Physics::CollisionManifold& hit)
	{
		if (!m_OnCollisionEnd.valid())
			return;

		m_OnCollisionEnd.call(hit);
	}

	void LuaScript::CaptureFunctions()
	{
		// Link functions
		m_OnStart          = m_luaState["PRX"]["OnStart"];
		m_OnUpdate         = m_luaState["PRX"]["OnUpdate"];
		m_OnCollisionStart = m_luaState["PRX"]["OnCollisionStart"];
		m_OnCollisionEnd   = m_luaState["PRX"]["OnCollisionEnd"];
	}
}