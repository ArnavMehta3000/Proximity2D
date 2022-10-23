#pragma once
#include "Engine/EngineMain.h"
#include "Engine/Application.h"

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
		virtual void OnRender(F32 dt) noexcept override final;
		virtual void OnShutdown() noexcept override final;

		void OnGUI() noexcept;
	};
}