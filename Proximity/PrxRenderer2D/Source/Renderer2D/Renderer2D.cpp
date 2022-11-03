#include "renderer2dpch.h"
#include "Renderer2D/Renderer2D.h"

namespace Proximity::Graphics
{
	Renderer2D* Renderer2D::s_instance = nullptr;

	Renderer2D* Renderer2D::Get()
	{
		if (s_instance == nullptr)
			return new Renderer2D();
		else
			return s_instance;
	}
	void Renderer2D::ShutdownAndReset()
	{
		s_instance->Shutdown();
		delete s_instance;
	}

	Renderer2D::Renderer2D()
		:
		m_d3d(nullptr),
		m_rasterizerStates  (std::vector<RasterizerState>  (Defaults::RASTERIZER_STATE_COUNT)),
		m_depthStencilStates(std::vector<DepthStencilState>(Defaults::DEPTH_STENCIL_STATE_COUNT)),
		m_blendStates       (std::vector<BlendState>       (Defaults::BLEND_STATE_COUNT)),
		m_samplerStates     (std::vector<SamplerState>     (Defaults::SAMPLER_STATE_COUNT))
	{
	}
	Renderer2D::~Renderer2D() {}

	bool Renderer2D::Init(HWND hWnd, U32 width, U32 height, bool isVsync)
	{
		m_d3d = new D3DManager();
		if (!m_d3d)
		{
			PRX_LOG_FATAL("Failed create D3DManager");
			return false;
		}

		m_d3d->Init(width, height, isVsync, hWnd, DXGI_FORMAT_R16G16B16A16_FLOAT);
		return true;
	}
}