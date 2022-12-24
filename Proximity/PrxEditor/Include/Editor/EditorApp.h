#pragma once
#include "Engine/EngineMain.h"
#include "Editor/Panels/ScenePanel.h"
#include "Editor/Panels/ViewportPanel2D.h"

using namespace Proximity::Math;
namespace Proximity::Editor
{
	class EditorApp : public Proximity::Core::Application
	{
	public:
		EditorApp(HINSTANCE hInst);
		~EditorApp();


	private:
		virtual void OnStart()      noexcept override final;
		virtual void OnTick(F32 dt) noexcept override final;
		virtual void OnRender()     noexcept override final;
		virtual void OnUI()         noexcept override final;
		virtual void OnShutdown()   noexcept override final;

		void CloseEditor();

		void SetupImGui();
		void OnImguiResize(Math::U32 width, Math::U32 height);
		void DrawImGuiMenuBar();
		void DrawImGuiAppTimeInfo();


		void SetImGuiStyleDeepDark();
		void SetImGuiStyleVS();
		void SetImGuiStyleRed();
		void SetImGuiStyleBlueGray();
		void SetImGuiStyleLightDark();

	private:
		bool m_showAppStatsWindow;
		std::vector<Panels::EditorPanel*> m_editorPanels;
	};
}