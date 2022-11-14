#pragma once
#include "Graphics/D3DManager.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Rendering/RenderingStructures.h"
#include "Graphics/Rendering/RenderCommands.h"

namespace Proximity::Graphics
{
	class Renderer2D : public Utils::IService
	{
	public:
		Renderer2D();
		~Renderer2D();

		const D3DManager* GetD3D() const noexcept { return m_d3d; }

		bool Init(HWND hWnd, Math::U32 width, Math::U32 height, bool isVsync);
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
		PipelineState m_pipelineState;
		PipelineState m_prevPiplineState;

		DepthTarget  m_depthTarget;
		
		std::vector<RenderTarget>      m_renderTargets;
		std::vector<RasterizerState>   m_rasterizerStates;
		std::vector<DepthStencilState> m_depthStencilStates;
		std::vector<BlendState>        m_blendStates;
		std::vector<SamplerState>      m_samplerStates;

		D3DManager* m_d3d;
	};
}