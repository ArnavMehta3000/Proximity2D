#pragma once
#include "Scripting/ScriptLink.h"

namespace Proximity::Modules
{
	class ScriptLibrary : public Utils::IService
	{
		using ScriptMap = std::unordered_map<std::string, std::shared_ptr<Scripting::ScriptLink>>;
	public:
		ScriptLibrary() = default;

		void InitProjectLib();
		void Refresh();

		void AddScriptLink(const std::shared_ptr<Scripting::ScriptLink>& src);
		void RemoveAudioSource(const std::string& scriptName);
		bool Exists(const std::string& scriptName);
		std::shared_ptr<Scripting::ScriptLink> Get(const std::string scriptName);

		inline const Math::U64 Count()  const noexcept { return m_scripts.size(); }
		inline const ScriptMap& GetMap() const noexcept { return m_scripts; }
		inline void UpdateSelected(std::string_view shader) { m_selectedScriptPath = shader; OnScriptSelected(shader); }
		inline const std::string& GetSelectedName() const noexcept { return m_selectedScriptPath; }

		Utils::Action<std::string_view> OnScriptSelected;

	private:
		std::string m_selectedScriptPath;
		ScriptMap   m_scripts;
	};
}