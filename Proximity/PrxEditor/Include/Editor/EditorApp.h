#pragma once
#include "Engine/EngineMain.h"
#include <PrxEngine.h>

using namespace Proximity::Math;
namespace Proximity::Editor
{
	class EditorApp : public Proximity::Core::Application
	{
	public:
		EditorApp(HINSTANCE hInst);
		~EditorApp();


	private:
		virtual void OnStart() noexcept override final;
		virtual void OnTick(F32 dt) noexcept override final;
		virtual void OnRender() noexcept override final;
		virtual void OnPostRender() noexcept override final;
		virtual void OnShutdown() noexcept override final;

		void SetupImGui();
		

		void OnGUI() noexcept;
	};
}