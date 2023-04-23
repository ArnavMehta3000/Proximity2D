#pragma once

namespace Proximity::Modules
{
	class ScriptLibrary : public Utils::IService
	{
		
	public:
		ScriptLibrary() = default;

		void InitProjectLib();

		const std::vector<std::string>& GetScriptsPathList() const { return m_scripts; }
		const std::string& GetSelectedScriptPath() const { return m_selectedScriptPath; }
		inline void SetSelectedScriptPath(const std::string_view s) { m_selectedScriptPath = s; OnScriptSelected(s); }

		Utils::Action<std::string_view> OnScriptSelected;

	private:
		std::string m_selectedScriptPath;
		std::vector<std::string> m_scripts;
	};
}