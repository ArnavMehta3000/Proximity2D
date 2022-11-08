#pragma once
namespace Proximity::Graphics
{
	struct Texture2DDesc
	{
		Math::F32   Width;
		Math::F32   Height;
		DXGI_FORMAT Format;
		Math::U32   BindFlags;
		void*       Data;
		Math::I32   DataPitch;
		Math::I32   DataSlicePitch;
		Math::I32   ArraySize;
		Math::U32   CPUAccess;

		Texture2DDesc();
	};

	struct Texture2D
	{
	public:
		Texture2D(){}


		ComPtr<ID3D11ShaderResourceView> m_SRV;
		ComPtr<ID3D11Texture2D> m_Tex2D;
		
		Math::U32 m_width;
		Math::U32 m_height;

		std::string m_name;
	};
}