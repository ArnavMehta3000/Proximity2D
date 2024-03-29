#pragma once
#include "Graphics/Rendering/RenderingEnums.h"

namespace Proximity::Graphics
{
	using RasterizerState    = ComPtr<ID3D11RasterizerState>;
	using DepthStencilState  = ComPtr<ID3D11DepthStencilState>;
	using BlendState         = ComPtr<ID3D11BlendState>;
	using D3DTex2D           = ComPtr<ID3D11Texture2D>;
	using ShaderResourceView = ComPtr<ID3D11ShaderResourceView>;
	using RenderTargetView   = ComPtr<ID3D11RenderTargetView>;
	using DepthStencilView   = ComPtr<ID3D11DepthStencilView>;
	
	
	struct SamplerState
	{
		std::string Name;
		ComPtr<ID3D11SamplerState> Sampler;
	};


	struct Texture2D
	{
	public:
		Texture2D() = default;
		bool CreateTexture(const void* data, Math::U32 sysMemPitch);
		bool CreateTexture();
		bool CreateSRV();
		void Release();

	public:
		std::string        Name;
		std::string        Filepath;
		Math::U32          Width;
		Math::U32          Height;
		Math::U32          BindFlags;
		DXGI_FORMAT        TexFormat;
		DXGI_FORMAT        SrvFormat;

		D3DTex2D           D3DTexture2D = nullptr;
		ShaderResourceView SRV = nullptr;
	};



	class RenderTarget
	{
	public:
		RenderTarget() = default;
		bool CreateRTV();
		void Release();
		void Resize(Math::U32 width, Math::U32 height);

	public:
		DXGI_FORMAT      RtvFormat;
		RenderTargetView RTV = nullptr;
		Texture2D        Texture;
	};

	class DepthTarget
	{
	public:
		DepthTarget() = default;
		bool CreateDSV();  
		void Release();
		void Resize(Math::U32 width, Math::U32 height);

	public:
		DXGI_FORMAT      DsvFormat;
		DepthStencilView DSV;
		Texture2D        Texture;
	};
}