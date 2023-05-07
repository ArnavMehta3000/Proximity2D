#pragma once
#include "Scripting/LuaScript.h"
#include <stack>

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
		void OnMouseLBDown();
		void OnMouseRBDown();
		void OnMouseMBDown();
		void OnMouseLBUp();
		void OnMouseRBUp();
		void OnMouseMBUp();

	private:
		Core::Entity*                     m_linkedEntity;
		sol::table                        m_entityTable;
		LuaScript                         m_script;
		std::stack<std::function<void()>> m_inputStack;
	};
}