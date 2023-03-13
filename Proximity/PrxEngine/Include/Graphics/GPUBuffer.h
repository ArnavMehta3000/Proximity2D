#pragma once
#include "Engine/BufferStructures.h"

namespace Proximity::Graphics
{
	// Wrapper class for constant buffer
	template <typename T>
	class GPUBuffer
	{
	public:
		GPUBuffer() = default;

		GPUBuffer(ID3D11DeviceContext* ctx)
			:
			m_context(ctx),
			m_buffer(nullptr),
			Data()
		{}

		GPUBuffer(ID3D11DeviceContext* ctx, const T data)
			:
			m_context(ctx),
			m_buffer(nullptr),
			Data(data)
		{}

		~GPUBuffer() { COM_RELEASE(m_buffer); }

		const ComPtr<ID3D11Buffer> GetBuffer() const noexcept { return m_buffer; }

		static GPUBuffer<T>* Create(ID3D11Device* dev, ID3D11DeviceContext* ctx, D3D11_USAGE usage = D3D11_USAGE_DYNAMIC, UINT cpuAccessFlags = D3D11_CPU_ACCESS_WRITE, UINT miscFlags = 0)
		{
			GPUBuffer<T>* cb = new GPUBuffer<T>(ctx);

			CREATE_ZERO(D3D11_BUFFER_DESC, desc);
			desc.Usage               = usage;
			desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags      = cpuAccessFlags;
			desc.MiscFlags           = miscFlags;
			desc.ByteWidth           = static_cast<UINT>(sizeof(T)) + (16 - sizeof(T) % 16);  // For 16 byte alignment
			desc.StructureByteStride = 0;

			if (FAILED(dev->CreateBuffer(&desc, 0, cb->m_buffer.ReleaseAndGetAddressOf())))
			{
				PRX_LOG_ERROR("Failed to create constant buffer!");
				return nullptr;
			}
			else
			{
				PRX_LOG_INFO("Created constant buffer of size: %u", static_cast<UINT>(sizeof(T)) + (16 - sizeof(T) % 16));
				return cb;
			}
		}

		bool ApplyChanges()
		{
			D3D11_MAPPED_SUBRESOURCE map{};
			HRESULT hr = m_context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
			if (FAILED(hr))
			{
				PRX_LOG_ERROR("Failed to map constant buffer subresource data");
				return false;
			}

			CopyMemory(map.pData, &Data, sizeof(T));
			m_context->Unmap(m_buffer.Get(), 0);

			return true;
		}

	public:
		T Data;

	private:
		ID3D11DeviceContext* m_context;
		ComPtr<ID3D11Buffer> m_buffer;
	};

	template <class T>
	using GPUBufferPtr = std::unique_ptr<Graphics::GPUBuffer<T>>;
}