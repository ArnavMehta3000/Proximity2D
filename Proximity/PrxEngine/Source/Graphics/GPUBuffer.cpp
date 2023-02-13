#include "enginepch.h"
#include "Graphics/GPUBuffer.h"

namespace Proximity::Graphics
{
	GPUBuffer::GPUBuffer()
	{
	}

	void GPUBuffer::Release()
	{
		COM_RELEASE(m_buffer);
	}
}