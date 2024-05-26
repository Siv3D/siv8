//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "D3D11VertexBufferManager2D.hpp"
# include "D3D11Renderer2DCommandManager.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		static void Resize(Array<Vertex2D>& vertexArray, const size_t vertexArrayWritePosTarget)
		{
			const size_t newVertexArraySize = std::bit_ceil(vertexArrayWritePosTarget);
			
			LOG_TRACE(fmt::format("ℹ️ Resized D3D11VertexBufferManager2D::m_vertexArray (size: {} -> {})", vertexArray.size(), newVertexArraySize));
			
			vertexArray.resize(newVertexArraySize);
		}

		static void Resize(Array<Vertex2D::IndexType>& indexArray, const size_t indexArrayWritePosTarget)
		{
			const size_t newIndexArraySize = std::bit_ceil(indexArrayWritePosTarget);

			LOG_TRACE(fmt::format("ℹ️ Resized D3D11VertexBufferManager2D::m_indexArray (size: {} -> {})", indexArray.size(), newIndexArraySize));

			indexArray.resize(newIndexArraySize);
		}
	}

	D3D11VertexBufferManager2D::D3D11VertexBufferManager2D()
		: m_vertexArray(InitialVertexArraySize)
		, m_indexArray(InitialIndexArraySize)
		, m_batches(1) {}

	bool D3D11VertexBufferManager2D::init(ID3D11Device* device, ID3D11DeviceContext* context)
	{
		m_device	= device;
		m_context	= context;

		// Vertex Buffer
		{
			const D3D11_BUFFER_DESC desc =
			{
				.ByteWidth				= (sizeof(Vertex2D) * VertexBufferSize),
				.Usage					= D3D11_USAGE_DYNAMIC,
				.BindFlags				= D3D11_BIND_VERTEX_BUFFER,
				.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE,
				.MiscFlags				= 0,
				.StructureByteStride	= 0, 
			};

			if (FAILED(m_device->CreateBuffer(&desc, nullptr, &m_vertexBuffer)))
			{
				return false;
			}
		}

		// Index Buffer
		{
			const D3D11_BUFFER_DESC desc =
			{
				.ByteWidth				= (sizeof(Vertex2D::IndexType) * IndexBufferSize),
				.Usage					= D3D11_USAGE_DYNAMIC,
				.BindFlags				= D3D11_BIND_INDEX_BUFFER,
				.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE,
				.MiscFlags				= 0,
				.StructureByteStride	= 0,
			};

			if (FAILED(m_device->CreateBuffer(&desc, nullptr, &m_indexBuffer)))
			{
				return false;
			}
		}

		return true;
	}

	Vertex2DBufferPointer D3D11VertexBufferManager2D::requestBuffer(const uint16 vertexSize, const uint32 indexSize, D3D11Renderer2DCommandManager& commandManager)
	{
		// VB
		{
			const uint32 vertexArrayWritePosTarget = (m_vertexArrayWritePos + vertexSize);

			if (m_vertexArray.size() < vertexArrayWritePosTarget)
			{
				if (MaxVertexArraySize < vertexArrayWritePosTarget)
				{
					return{ nullptr, 0, 0 };
				}

				Resize(m_vertexArray, vertexArrayWritePosTarget);
			}
		}

		// IB
		{
			const uint32 indexArrayWritePosTarget = (m_indexArrayWritePos + indexSize);

			if (m_indexArray.size() < indexArrayWritePosTarget)
			{
				if (MaxIndexArraySize < indexArrayWritePosTarget)
				{
					return{ nullptr, 0, 0 };
				}

				Resize(m_indexArray, indexArrayWritePosTarget);
			}
		}
		
		BatchBufferPos* pLastBatch = &m_batches.back();
		{
			if ((VertexBufferSize < (pLastBatch->vertexPos + vertexSize)
				|| (IndexBufferSize < (pLastBatch->indexPos + indexSize))))
			{
				commandManager.pushUpdateBuffers(static_cast<uint32>(m_batches.size()));
				m_batches.emplace_back();
				pLastBatch = &m_batches.back();
			}
		}

		const Vertex2DBufferPointer result
		{
			.pVertex		= (m_vertexArray.data() + m_vertexArrayWritePos),
			.pIndex			= (m_indexArray.data() + m_indexArrayWritePos),
			.indexOffset	= static_cast<Vertex2D::IndexType>(pLastBatch->vertexPos),
		};

		m_vertexArrayWritePos	+= vertexSize;
		m_indexArrayWritePos	+= indexSize;

		pLastBatch->vertexPos	+= vertexSize;
		pLastBatch->indexPos	+= indexSize;

		return result;
	}

	size_t D3D11VertexBufferManager2D::num_batches() const noexcept
	{
		return m_batches.size();
	}

	void D3D11VertexBufferManager2D::reset()
	{
		m_batches.assign(1, BatchBufferPos{});
		m_vertexArrayWritePos	= 0;
		m_indexArrayWritePos	= 0;
	}

	void D3D11VertexBufferManager2D::setBuffers()
	{
		ID3D11Buffer* const pBuf[1] = { m_vertexBuffer.Get() };
		constexpr UINT Stride = sizeof(Vertex2D);
		constexpr UINT Offset = 0;
		m_context->IASetVertexBuffers(0, 1, pBuf, &Stride, &Offset);

		constexpr DXGI_FORMAT IndexFormat = ((sizeof(Vertex2D::IndexType) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT);
		m_context->IASetIndexBuffer(m_indexBuffer.Get(), IndexFormat, 0);
	}

	BatchInfo2D D3D11VertexBufferManager2D::commitBuffers(const size_t batchIndex)
	{
		assert(batchIndex < m_batches.size());

		size_t vertexArrayReadPos	= 0;
		size_t indexArrayReadPos	= 0;

		for (size_t i = 0; i < batchIndex; ++i)
		{
			const auto& batch	= m_batches[i];
			vertexArrayReadPos	+= batch.vertexPos;
			indexArrayReadPos	+= batch.indexPos;
		}

		BatchInfo2D batchInfo;
		const auto& currentBatch = m_batches[batchIndex];

		// VB
		if (const uint32 vertexSize = currentBatch.vertexPos)
		{
			const Vertex2D* pSrc = (m_vertexArray.data() + vertexArrayReadPos);

			D3D11_MAP mapType = D3D11_MAP_WRITE_NO_OVERWRITE;

			if (VertexBufferSize < (m_vertexBufferWritePos + vertexSize))
			{
				mapType = D3D11_MAP_WRITE_DISCARD;
				m_vertexBufferWritePos = 0;
			}

			D3D11_MAPPED_SUBRESOURCE res;

			if (SUCCEEDED(m_context->Map(m_vertexBuffer.Get(), 0, mapType, 0, &res)))
			{
				if (Vertex2D* const p = static_cast<Vertex2D*>(res.pData))
				{
					Vertex2D* const pDst = (p + m_vertexBufferWritePos);		
					std::memcpy(pDst, pSrc, sizeof(Vertex2D) * vertexSize);
				}

				m_context->Unmap(m_vertexBuffer.Get(), 0);
			}

			batchInfo.baseVertexLocation = m_vertexBufferWritePos;
			m_vertexBufferWritePos += vertexSize;
		}

		// IB
		if (const uint32 indexSize = currentBatch.indexPos)
		{
			const Vertex2D::IndexType* pSrc = (m_indexArray.data() + indexArrayReadPos);

			D3D11_MAP mapType = D3D11_MAP_WRITE_NO_OVERWRITE;

			if (IndexBufferSize < (m_indexBufferWritePos + indexSize))
			{
				mapType = D3D11_MAP_WRITE_DISCARD;
				m_indexBufferWritePos = 0;
			}

			D3D11_MAPPED_SUBRESOURCE res;

			if (SUCCEEDED(m_context->Map(m_indexBuffer.Get(), 0, mapType, 0, &res)))
			{
				if (Vertex2D::IndexType* const p = static_cast<Vertex2D::IndexType*>(res.pData))
				{
					Vertex2D::IndexType* const pDst = p + m_indexBufferWritePos;
					std::memcpy(pDst, pSrc, (sizeof(Vertex2D::IndexType) * indexSize));
				}

				m_context->Unmap(m_indexBuffer.Get(), 0);
			}

			batchInfo.indexCount = indexSize;
			batchInfo.startIndexLocation = m_indexBufferWritePos;
			m_indexBufferWritePos += indexSize;
		}

		return batchInfo;
	}
}
