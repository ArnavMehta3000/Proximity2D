#include "enginepch.h"
#include "Graphics/Rendering/RenderingStructures.h"
#include "Utils/ServiceLocator.h"
#include "Engine/EngineGlobals.h"
#include "Graphics/D3DManager.h"

namespace Proximity::Graphics
{
	bool Texture2D::CreateTexture()
	{
		CREATE_ZERO(D3D11_TEXTURE2D_DESC, desc);
		desc.Width            = Width;
		desc.Height           = Height;
		desc.ArraySize        = 1;
		desc.MipLevels        = 1;
		desc.Format           = TexFormat;
		desc.Usage            = D3D11_USAGE_DEFAULT;
		desc.BindFlags        = BindFlags;
		desc.CPUAccessFlags   = 0;
		desc.SampleDesc.Count = 1;
		desc.MiscFlags        = 0;

		auto d3d   = Core::Globals::g_engineServices.ResolveService<Graphics::D3DManager>();
		HRESULT hr = d3d->GetDevice()->CreateTexture2D(&desc, nullptr, D3DTexture2D.ReleaseAndGetAddressOf());
		PRX_FAIL_HR(hr);
		
		return true;
	}

	bool Texture2D::CreateSRV()
	{
		CREATE_ZERO(D3D11_SHADER_RESOURCE_VIEW_DESC, desc);
		desc.Format                    = SrvFormat;
		desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		desc.Texture2D.MipLevels       = 1;
		desc.Texture2D.MostDetailedMip = 0;

		auto d3d   = Core::Globals::g_engineServices.ResolveService<Graphics::D3DManager>();
		HRESULT hr = d3d->GetDevice()->CreateShaderResourceView(D3DTexture2D.Get(), &desc, SRV.ReleaseAndGetAddressOf());
		PRX_FAIL_HR(hr);

		return true;
	}

	void Texture2D::Release()
	{
		COM_RELEASE(SRV);
		COM_RELEASE(D3DTexture2D);
	}



	bool RenderTarget::CreateRTV()
	{
		CREATE_ZERO(D3D11_RENDER_TARGET_VIEW_DESC, desc);
		desc.Format             = RtvFormat;
		desc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		desc.Texture2D.MipSlice = 0;

		auto d3d = Core::Globals::g_engineServices.ResolveService<Graphics::D3DManager>();
		HRESULT hr = d3d->GetDevice()->CreateRenderTargetView(Texture.D3DTexture2D.Get(), &desc, RTV.ReleaseAndGetAddressOf());
		PRX_FAIL_HR(hr);

		return true;
	}

	void RenderTarget::Release()
	{
		COM_RELEASE(RTV);
		Texture.Release();
	}



	void DepthTarget::Release()
	{
		COM_RELEASE(DSV);
		Texture.Release();
	}
}