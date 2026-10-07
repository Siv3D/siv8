//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
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

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	D3D11VertexBufferManager2D::D3D11VertexBufferManager2D()
		: m_vertexArray(InitialVertexArraySize)
		, m_indexArray(InitialIndexArraySize)
		, m_batches(1) {}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	requestBuffer
	//
	////////////////////////////////////////////////////////////////

	Vertex2DBufferPointer D3D11VertexBufferManager2D::requestBuffer(const uint16 vertexCount, const uint32 indexCount, D3D11Renderer2DCommandManager& commandManager)
	{
		// 描画データを書き込む CPU 配列の領域を予約し、その先頭ポインタを返す。
		// GPU への転送はここでは行わず、後で commitBuffers() がバッチ単位で行う。
		// バッチは、固定サイズの GPU 頂点・インデックスバッファに収まるデータのまとまり。

		// 1 回の要求を複数バッチには分割しないため、単独で GPU バッファを超える要求は拒否する。
		// 頂点数は引数が uint16 なので、VertexBufferSize（65,535）以下に収まる。
		if (IndexBufferSize < indexCount)
		{
			return{ nullptr, nullptr, 0 };
		}

		// CPU 側の頂点配列を必要に応じて拡張する。複数バッチのデータを連続して保持するため、
		// 配列全体の上限 MaxVertexArraySize と、1 バッチの上限 VertexBufferSize は別に扱う。
		{
			// CPU 配列全体での「今回の予約領域の末尾の次」。単位はバイトではなく頂点数。
			const uint32 vertexArrayWritePosTarget = (m_vertexArrayWritePos + vertexCount);

			if (m_vertexArray.size() < vertexArrayWritePosTarget)
			{
				// CPU 配列の上限を超える場合は、書き込み位置を進めずに失敗を返す。
				if (MaxVertexArraySize < vertexArrayWritePosTarget)
				{
					return{ nullptr, 0, 0 };
				}

				// 小さな要求のたびに再確保しないよう、必要数以上の最小の 2 の累乗へ拡張する。
				Resize(m_vertexArray, vertexArrayWritePosTarget);
			}
		}

		// CPU 側のインデックス配列も同様に拡張する。こちらの位置と個数はインデックス単位。
		{
			const uint32 indexArrayWritePosTarget = (m_indexArrayWritePos + indexCount);

			if (m_indexArray.size() < indexArrayWritePosTarget)
			{
				if (MaxIndexArraySize < indexArrayWritePosTarget)
				{
					return{ nullptr, 0, 0 };
				}

				Resize(m_indexArray, indexArrayWritePosTarget);
			}
		}
		
		// 現在のバッチに追加できるかを調べる。vertexPos / indexPos は、
		// CPU 配列全体ではなく、そのバッチ内での使用数（次の書き込み位置）を表す。
		BatchBufferPos* pLastBatch = &m_batches.back();
		{
			if ((VertexBufferSize < (pLastBatch->vertexPos + vertexCount)
				|| (IndexBufferSize < (pLastBatch->indexPos + indexCount))))
			{
				// 頂点・インデックスのどちらかが収まらなければ、両方とも新しいバッチへ移る。
				// バッチ内の位置は 0 から始まるが、CPU 配列への書き込みは続きに追加する。
				commandManager.deferUpdateBuffers(static_cast<uint32>(m_batches.size()));
				m_batches.emplace_back();
				// emplace_back() で配列が再確保される可能性があるため、ポインタを取り直す。
				pLastBatch = &m_batches.back();
			}
		}

		// 呼び出し側が頂点とインデックスを書き込む場所は、CPU 配列全体での位置から求める。
		// 一方、インデックスに加える indexOffset は、このバッチの先頭頂点からの相対位置。
		// GPU 上でのバッチの配置位置は、描画時に別途 baseVertexLocation として加算される。
		const Vertex2DBufferPointer result
		{
			.pVertex		= (m_vertexArray.data() + m_vertexArrayWritePos),
			.pIndex			= (m_indexArray.data() + m_indexArrayWritePos),
			.indexOffset	= static_cast<Vertex2D::IndexType>(pLastBatch->vertexPos),
		};

		// 予約した分だけ CPU 配列全体の位置を進め、次の要求と領域が重ならないようにする。
		m_vertexArrayWritePos	+= vertexCount;
		m_indexArrayWritePos	+= indexCount;

		// バッチ内の使用数も進める。次回の収容判定と、バッチ内の相対位置に使う。
		pLastBatch->vertexPos	+= vertexCount;
		pLastBatch->indexPos	+= indexCount;

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_batches
	//
	////////////////////////////////////////////////////////////////

	size_t D3D11VertexBufferManager2D::num_batches() const noexcept
	{
		return m_batches.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	reset
	//
	////////////////////////////////////////////////////////////////

	void D3D11VertexBufferManager2D::reset()
	{
		m_batches.assign(1, BatchBufferPos{});
		m_vertexArrayWritePos	= 0;
		m_indexArrayWritePos	= 0;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setBuffers
	//
	////////////////////////////////////////////////////////////////

	void D3D11VertexBufferManager2D::setBuffers()
	{
		ID3D11Buffer* const pBuf[1] = { m_vertexBuffer.Get() };
		constexpr UINT Stride = sizeof(Vertex2D);
		constexpr UINT Offset = 0;
		m_context->IASetVertexBuffers(0, 1, pBuf, &Stride, &Offset);

		constexpr DXGI_FORMAT IndexFormat = ((sizeof(Vertex2D::IndexType) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT);
		m_context->IASetIndexBuffer(m_indexBuffer.Get(), IndexFormat, 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	commitBuffers
	//
	////////////////////////////////////////////////////////////////

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
		if (const uint32 vertexCount = currentBatch.vertexPos)
		{
			const Vertex2D* pSrc = (m_vertexArray.data() + vertexArrayReadPos);

			D3D11_MAP mapType = D3D11_MAP_WRITE_NO_OVERWRITE;

			if (VertexBufferSize < (m_vertexBufferWritePos + vertexCount))
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
					std::memcpy(pDst, pSrc, (sizeof(Vertex2D) * vertexCount));
				}

				m_context->Unmap(m_vertexBuffer.Get(), 0);
			}

			batchInfo.baseVertexLocation = m_vertexBufferWritePos;
			m_vertexBufferWritePos += vertexCount;
		}

		// IB
		if (const uint32 indexCount = currentBatch.indexPos)
		{
			const Vertex2D::IndexType* pSrc = (m_indexArray.data() + indexArrayReadPos);

			D3D11_MAP mapType = D3D11_MAP_WRITE_NO_OVERWRITE;

			if (IndexBufferSize < (m_indexBufferWritePos + indexCount))
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
					std::memcpy(pDst, pSrc, (sizeof(Vertex2D::IndexType) * indexCount));
				}

				m_context->Unmap(m_indexBuffer.Get(), 0);
			}

			batchInfo.indexCount = indexCount;
			batchInfo.startIndexLocation = m_indexBufferWritePos;
			m_indexBufferWritePos += indexCount;
		}

		return batchInfo;
	}
}
