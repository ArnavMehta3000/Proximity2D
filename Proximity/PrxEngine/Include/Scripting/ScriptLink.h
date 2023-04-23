#pragma once

namespace Proximity::Scripting
{
	class ScriptLink : public Utils::IService
	{
	public:
		ScriptLink() = default;

		void Init();
		void Shutdown();
	};
}