#pragma once
#include "Graphics/D3DManager.h"
#include "Graphics/Rendering/RenderingStructures.h"
#include "Engine/Components/Components.h"
#include "Graphics/VertexStructures.h"




namespace Proximity::Graphics
{
	class Renderer2D : public Utils::IService
	{
	public:
		Renderer2D();
		~Renderer2D();

		const D3DManager* GetD3D() const noexcept { return m_d3d; }
		RenderTarget& GetEditorFrameBuffer() noexcept { return m_editorFrameBuffer; }
		bool Init(HWND hWnd, Math::U32 width, Math::U32 height, bool isVsync);
		void Shutdown();

		void Resize(Math::U32 width, Math::U32 height);
		inline constexpr Math::U32 GetBatchSize() const noexcept { return m_batchSize; }

	public:  // All public rendering functions
		void EndFrame();  // Present to the screen 
		void BindRenderTarget(const RenderTargetType& rtType);
		void DrawQuad();

		void DrawQuad();
		
	private:  // All private intialization functions go here
		bool InitInternal();
		bool CreateRenderAndDepthTarget();
		bool CreateRasterizerStates();
		bool CreateBlendStates();
		bool CreateSamplerStates();

	private:
		DepthTarget       m_depthTarget;
		RenderTargetView  m_backBuffer;
		RenderTarget      m_editorFrameBuffer;
		RenderTarget      m_gameFrameBuffer;
		DepthStencilState m_depthStencilState;
		
		std::vector<RasterizerState>   m_rasterizerStates;
		std::vector<BlendState>        m_blendStates;
		std::vector<SamplerState>      m_samplerStates;

		D3DManager* m_d3d;

		bool m_resizing;
	};
}