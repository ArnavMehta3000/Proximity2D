#pragma once
#include "Renderer2D/D3DManager.h"
#include "Renderer2D/RenderingEnums.h"

namespace Proximity::Graphics
{
	class Renderer2D
	{
	public:
		static Renderer2D* Get();
		static void ShutdownAndReset();

		bool Init(HWND hWnd);
		void Shutdown();

	private:
		static Renderer2D* s_instance;
		Renderer2D();
		~Renderer2D();
		
		
		D3DManager* m_d3d;
	};
}
#define RENDERER2D Proximity::Graphics::Renderer2D::Get()