#pragma once

namespace Proximity::Graphics
{
	class Material
	{
	public:
		Material();
		Material(const Material& mat) = default;
		~Material();

	private:
		ComPtr<ID3D11VertexShader> m_vertexShader;
		ComPtr<ID3D11PixelShader> m_pixelShader;

	};
}