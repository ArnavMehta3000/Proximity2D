#pragma once
#include "Engine/EngineMain.h"
#include "Editor/Panels/EditorPanel.h"

using namespace Proximity::Math;
namespace Proximity::Editor
{
	class EditorApp : public Proximity::Core::Application
	{
	private:
		enum class SceneState
		{
			Edit = 0,
			Play = 1
		};

		
	public:
		EditorApp(HINSTANCE hInst);
		~EditorApp() = default;


	private:
		virtual void OnStart()      noexcept override final;
		virtual void OnTick(F32 dt) noexcept override final;
		virtual void OnRender()     noexcept override final;
		virtual void OnUI()         noexcept override final;
		virtual void OnShutdown()   noexcept override final;

		void CloseEditor();

		void SetupImGui();
		void DrawImGuiMenuBar();
		void DrawImGuiProjectWindow();
		void DrawImGuiAppTimeInfo();


		void OnScenePlay();
		void OnSceneStop();


		void SetImGuiStyleDeepDark();
		void SetImGuiStyleVS();
		void SetImGuiStyleRed();
		void SetImGuiStyleBlueGray();
		void SetImGuiStyleLightDark();
		void SetImGuiStyleDracula();


	private:
		bool                              m_showAppStatsWindow;
		std::vector<Panels::EditorPanel*> m_editorPanels;
		Core::OrthographicCamera          m_editorCam;
		SceneState                        m_sceneState;
	};
}