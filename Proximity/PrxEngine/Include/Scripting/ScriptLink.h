#pragma once

namespace Proximity::Scripting
{
	struct ScriptLinkData;

	class ScriptLink : public Utils::IService
	{
	public:
		ScriptLink() = default;

		const ScriptLinkData* GetData();

		void Init();
		void Shutdown();
	};
}