#pragma once
#include "Engine/Game/Entity.h"

namespace Proximity::Core
{
	class ScriptableEntity
	{
		friend class Scene;

	public:
		virtual ~ScriptableEntity() {}

		template <typename T>
		T& GetComponent()
		{
			return m_entity.GetComponent<T>();
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(Math::F32 dt) {}

	private:
		Core::Entity m_entity;
	};

}