#pragma once
#include "Graphics/D3DManager.h"
#include "Graphics/Texture2D.h"
#include "Graphics/Rendering/RenderingStructures.h"

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

		void Resize(Math::U32 width, Math::U32 height);

	public:  // All public rendering functions
		void EndFrame();  // Present to the screen 

		const RenderTarget& GetFrameBuffer() const noexcept { return m_frameBuffer; }

		void BindRenderTarget(const RenderTargetType& rtType);
		

	private:  // All private intialization functions go here
		bool InitInternal();
		bool CreateRenderAndDepthTarget();
		bool CreateRasterizerStates();
		bool CreateBlendStates();
		bool CreateSamplerStates();

	private:
		PipelineState m_pipelineState;
		PipelineState m_prevPiplineState;

		DepthTarget       m_depthTarget;
		RenderTarget      m_backBuffer;
		RenderTarget      m_frameBuffer;
		DepthStencilState m_depthStencilState;
		
		std::vector<RasterizerState>   m_rasterizerStates;
		std::vector<BlendState>        m_blendStates;
		std::vector<SamplerState>      m_samplerStates;

		D3DManager* m_d3d;

		bool m_resizing;
	};
}