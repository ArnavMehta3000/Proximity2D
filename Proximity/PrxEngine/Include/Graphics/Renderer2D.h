#pragma once
#include "Graphics/D3DManager.h"
#include "Graphics/RenderingEnums.h"

namespace Proximity::Graphics
{
	class Renderer2D
	{
	public:
		static Renderer2D* Get();
		static void ShutdownAndReset();

		bool Init(HWND hWnd, U32 width, U32 height, bool isVsync);
		void Shutdown();

	private:
		static Renderer2D* s_instance;
		Renderer2D();
		~Renderer2D();
		
		std::vector<RasterizerState>   m_rasterizerStates;
		std::vector<DepthStencilState> m_depthStencilStates;
		std::vector<BlendState>        m_blendStates;
		std::vector<SamplerState>      m_samplerStates;
		D3DManager* m_d3d;
	};
}
#define RENDERER2D Proximity::Graphics::Renderer2D::Get()