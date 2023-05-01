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
	void ScriptLink::LinkEntity(const Core::Entity& e)
	{
		m_linkedEntity = new Core::Entity(e);
	}

	void ScriptLink::UnlinkEntity()
	{
		SAFE_DELETE(m_linkedEntity)
		
		m_entityTable = sol::nil;
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