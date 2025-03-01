//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "ConstantBuffer_D3D11.hpp"
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	ConstantBuffer_D3D11::ConstantBuffer_D3D11(const size_t size)
		: m_bufferSize{ static_cast<uint32>(size) } {}

	bool ConstantBuffer_D3D11::_internal_init()
	{
		if (m_buffer)
		{
			return true;
		}

		CRenderer_D3D11* pRenderer = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer));

		ID3D11Device* device = pRenderer->getDevice().getDevice();

		if (not device)
		{
			return false;
		}

		m_context = pRenderer->getDevice().getContext();

		const D3D11_BUFFER_DESC desc
		{
			.ByteWidth				= m_bufferSize,
			.Usage					= D3D11_USAGE_DYNAMIC,
			.BindFlags				= D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE,
			.MiscFlags				= 0,
			.StructureByteStride	= 0,
		};

		LOG_TRACE("ID3D11Device::CreateBuffer()");

		if (FAILED(device->CreateBuffer(&desc, nullptr, &m_buffer)))
		{
			LOG_FAIL("❌ ConstantBuffer_D3D11: ID3D11Device::CreateBuffer() failed");

			return false;
		}

		LOG_DEBUG(fmt::format("ConstantBuffer_D3D11 initialized (size: {0})", m_bufferSize));

		return true;
	}

	bool ConstantBuffer_D3D11::_internal_update(const void* data, const size_t size)
	{
		if (not _internal_init())
		{
			return false;
		}

		D3D11_MAPPED_SUBRESOURCE mapped;
		{
			if (FAILED(m_context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
			{
				return false;
			}

			if (not mapped.pData)
			{
				m_context->Unmap(m_buffer.Get(), 0);
				return false;
			}

			std::memcpy(mapped.pData, data, size);
			
			m_context->Unmap(m_buffer.Get(), 0);
		}

		return true;
	}
}
