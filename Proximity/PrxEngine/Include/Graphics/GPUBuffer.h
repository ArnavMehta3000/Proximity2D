#pragma once
#include "Graphics/GPUShader.h"

namespace Proximity::Graphics
{
	class GPUBuffer
	{
	public:
		GPUBuffer();
		void Release();

	private:
		ComPtr<ID3D11Buffer> m_buffer;
	};
}