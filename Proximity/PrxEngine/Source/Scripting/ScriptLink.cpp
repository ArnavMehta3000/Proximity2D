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
		
		CreateState(m_luaState);
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
		result();

		// Link functions
		m_OnCompile       = m_luaState["PRX"]["OnCompile"];
		m_OnStart         = m_luaState["PRX"]["OnStart"];
		m_OnUpdate        = m_luaState["PRX"]["OnUpdate"];
		m_OnKeyboardInput = m_luaState["PRX"]["OnKeyboardInput"];
		m_OnMouseInput    = m_luaState["PRX"]["OnMouseInput"];
		m_OnEnd           = m_luaState["PRX"]["OnEnd"];

		UpdateEntityLink();

		if (m_OnCompile.valid())
		{
			m_OnCompile.call();
		}

		return true;
	}

	void ScriptLink::CreateState(sol::state& lua)
	{
		lua = sol::state();

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

		// Setup include diretories
		std::filesystem::path path = Utils::DirectoryManager::s_appDirectories.ScriptsPath;
		path /= "?.lua";
		lua["package"]["path"] = path.string().c_str();



		// Set functions
		sol::table luaTable = lua.create_table();
		luaTable.set_function("Log", &ScriptLink::LogToEditor, this);
		luaTable.set_function("GetEntity", &ScriptLink::LuaGetEntity, this);

		// Set user types
		auto vec3Type = lua.new_usertype<Math::Vector3>(
			"Vector3",
			/*"x", &Math::Vector3::x,
			"y", &Math::Vector3::y,
			"z", &Math::Vector3::z,*/
			sol::constructors<
			Math::Vector3(),
			Math::Vector3(float, float, float),
			Math::Vector3(const Vector3&)
			>()
		);

		auto transformType = lua.new_usertype<Core::TransformComponent>(
			"Transform",
			/*"position" , &Core::TransformComponent::m_Position,
			"rotation" , &Core::TransformComponent::m_Rotation,
			"scale"    , &Core::TransformComponent::m_Scale,*/
			sol::constructors<
			Core::TransformComponent(),
			Core::TransformComponent(Math::Vector3),
			Core::TransformComponent(Math::Vector3, Math::Vector3),
			Core::TransformComponent(Math::Vector3, Math::Vector3, Math::Vector3),
			Core::TransformComponent(const Core::TransformComponent&)
			>()
		);

		lua["PRX"] = luaTable;
	}

	void ScriptLink::UpdateEntityLink()
	{
		if (m_linkedEntity == nullptr)
			return;

		
		/*auto& t = m_linkedEntity->GetComponent<Proximity::Core::TransformComponent>();

		m_entity["transform"]["position"]["x"] = t.m_Position.x;
		m_entity["transform"]["position"]["y"] = t.m_Position.y;
		m_entity["transform"]["position"]["z"] = t.m_Position.z;
		m_entity["transform"]["rotation"]["x"] = t.m_Rotation.x;
		m_entity["transform"]["rotation"]["y"] = t.m_Rotation.y;
		m_entity["transform"]["rotation"]["z"] = t.m_Rotation.z;
		m_entity["transform"]["scale"]["x"]    = t.m_Scale.x;
		m_entity["transform"]["scale"]["y"]    = t.m_Scale.y;
		m_entity["transform"]["scale"]["z"]    = t.m_Scale.z;*/

	}


	void ScriptLink::LinkEntity(Core::Entity& e)
	{
		auto& t = e.GetComponent<Proximity::Core::TransformComponent>();
		

		sol::table transform       = m_luaState.create_table();
		sol::table position        = m_luaState.create_table();
		sol::table rotation        = m_luaState.create_table();
		sol::table scale           = m_luaState.create_table();

		position["x"] = sol::property(
			[&t]() { return t.m_Position.x; },     // Get
			[&t](float x) { t.m_Position.x = x; }  // Set
		);
		position["y"] = sol::property(
			[&t]() { return t.m_Position.y; },     // Get
			[&t](float y) { t.m_Position.y = y; }  // Set
		);
		position["z"] = sol::property(
			[&t]() { return t.m_Position.z; },     // Get
			[&t](float z) { t.m_Position.z = z; }  // Set
		);
		rotation["x"] = sol::property(
			[&t]() { return t.m_Rotation.x; },     // Get
			[&t](float x) { t.m_Rotation.x = x; }  // Set
		);
		rotation["y"] = sol::property(
			[&t]() { return t.m_Rotation.y; },     // Get
			[&t](float y) { t.m_Rotation.y = y; }  // Set
		);
		rotation["z"] = sol::property(
			[&t]() { return t.m_Rotation.z; },     // Get
			[&t](float z) { t.m_Rotation.z = z; }  // Set
		);
		scale["x"]    = sol::property(
			[&t]() { return t.m_Scale.x; },     // Get
			[&t](float x) { t.m_Scale.x = x; }  // Set
		);
		scale["y"]    = sol::property(
			[&t]() { return t.m_Scale.y; },     // Get
			[&t](float y) { t.m_Scale.y = y; }  // Set
		);
		scale["z"]    = sol::property(
			[&t]() { return t.m_Scale.z; },     // Get
			[&t](float z) { t.m_Scale.z = z; }  // Set
		);

		transform["position"] = position;
		transform["rotation"] = rotation;
		transform["scale"]    = scale;

		// Try get other components
		
		// Create a Lua table to hold the entity data
		m_entity              = m_luaState.create_table();
		m_entity["name"]      = e.GetComponent<Proximity::Core::NameComponent>().m_EntityName;
		m_entity["transform"] = sol::property(
			[&transform]() { return transform; },  // Get
			[&t](const sol::table& newTransform)   // Set
			{
				t.m_Position = Math::Vector3(
					newTransform["position"]["x"],
					newTransform["position"]["y"],
					newTransform["position"]["z"]
				);

				t.m_Rotation = Math::Vector3(
					newTransform["rotation"]["x"],
					newTransform["rotation"]["y"],
					newTransform["rotation"]["z"]
				);

				t.m_Scale = Math::Vector3(
					newTransform["scale"]["x"],
					newTransform["scale"]["y"],
					newTransform["scale"]["z"]
				);
			}
		);

		m_hasEntity = true;
		m_linkedEntity = new Core::Entity(e);

		UpdateEntityLink();
	}

	void ScriptLink::UnlinkEntity()
	{
		delete m_linkedEntity;
		m_linkedEntity = nullptr;
		
		m_entity       = sol::nil;
		m_hasEntity    = false;
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
		if (m_hasEntity)
			if (!Compile())  // This resets the state and reinitialies all variables
				return;

		UpdateEntityLink();
		if (!m_OnStart.valid())
			return;
		
		m_OnStart();
	}

	void ScriptLink::CallOnUpdate(float dt)
	{
		UpdateEntityLink();
		if (!m_OnUpdate.valid())
			return;
		m_OnUpdate(dt);
	}

	void ScriptLink::CallOnEnd()
	{
		if (!m_OnEnd.valid())
			return;
		m_OnEnd();
	}

}