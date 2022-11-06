#pragma once
#include "Graphics/D3DManager.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Rendering/RenderingStructures.h"
#include "Graphics/Rendering/RenderingEnums.h"
#include "Graphics/Rendering/RenderCommands.h"

namespace Proximity::Graphics
{
	class Renderer2D
	{
	public:
		static Renderer2D* Get();
		static void ShutdownAndReset();

		const D3DManager* GetD3D() const noexcept { return m_d3d; }

		bool Init(HWND hWnd, U32 width, U32 height, bool isVsync);
		void Shutdown();

	public:  // All public rendering functions
		void BeginRendering(const ClearCommand& clrCmd);  // Clears render targets
		void BeginFrame();  // Reset rendering data
		void EndFrame();  // Present to the screen 


	private:  // All private intialization functions go here
		bool CreateRenderTargets();
		bool CreateRasterizerStates();
		bool CreateBlendStates();
		bool CreateSamplerStates();
		bool CreateDepthStencilStates();

	private:
		static Renderer2D* s_instance;
		Renderer2D();
		~Renderer2D();

		DepthTarget  m_depthTarget;
		
		std::vector<RenderTarget>      m_renderTargets;
		std::vector<RasterizerState>   m_rasterizerStates;
		std::vector<DepthStencilState> m_depthStencilStates;
		std::vector<BlendState>        m_blendStates;
		std::vector<SamplerState>      m_samplerStates;
		D3DManager* m_d3d;
	};
}
#define RENDERER2D Proximity::Graphics::Renderer2D::Get()