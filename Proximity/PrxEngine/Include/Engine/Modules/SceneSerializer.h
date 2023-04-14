#pragma once
#include "Engine/Game/Entity.h"

namespace Proximity::Modules
{
	class SceneSerializer
	{
	public:
		SceneSerializer(Core::Scene* scene);

		bool Serialize();
		Core::Scene* Deserialize(const std::string& filepath);


	private:
		Core::Scene* m_scene;
	};
}
