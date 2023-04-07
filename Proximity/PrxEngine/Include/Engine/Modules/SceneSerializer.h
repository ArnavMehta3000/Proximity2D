#pragma once
#include "Engine/Game/Entity.h"
#include <yaml-cpp/yaml.h>

namespace Proximity::Modules
{
	class SceneSerializer
	{
	public:
		SceneSerializer(Core::Scene* scene);

		bool Serialize();
		bool Deserialize(const std::string& filepath);


	private:
		Core::Scene* m_scene;
	};
}
