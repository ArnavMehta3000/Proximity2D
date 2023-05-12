#pragma once
#include "Scripting/LuaScript.h"
#include <queue>

namespace Proximity::Core
{
	class Entity;
}

namespace Proximity::Scripting
{
	class ScriptLink
	{
	public:
		ScriptLink();
		explicit ScriptLink(const std::string_view& scriptFilePath);
		~ScriptLink();

		const LuaScript& GetLuaScript() const noexcept;

		void LinkEntity(const Core::Entity& e);
		void UnlinkEntity();
		void EnableInput(bool enable);

		bool Compile();
		void CallOnStart();
		void CallOnUpdate(float dt);
		void CallOnCollisionStart(const Physics::CollisionManifold& hit);
		void CallOnCollisionEnd(const Physics::CollisionManifold& hit);

	private:
		sol::object GetEntity() const noexcept;

		void OnKeyboard(Core::Input::KeyInfo keyInfo);

	private:
		Core::Entity* m_linkedEntity;
		sol::table                        m_entityTable;
		LuaScript                         m_script;
	};
}