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

# include "MetalVertexBufferManager2D.hpp"

namespace s3d
{
	void MetalVertexBufferManager2D::init(MTL::Device* device)
	{
		for (auto& buffer: m_buffers)
		{
			{
				auto& vertexBuffer = buffer.vertexBuffer;
				vertexBuffer.buffer = NS::TransferPtr(device->newBuffer((sizeof(Vertex2D) * InitialVertexBufferSize), MTL::ResourceStorageModeShared));
				vertexBuffer.pointer = static_cast<Vertex2D*>(vertexBuffer.buffer->contents());
				vertexBuffer.size = InitialVertexBufferSize;
			}
			
			{
				auto& indexBuffer = buffer.indexBuffer;
				indexBuffer.buffer = NS::TransferPtr(device->newBuffer((sizeof(Vertex2D::IndexType) * InitialIndexBufferSize), MTL::ResourceStorageModeShared));
				indexBuffer.pointer = static_cast<Vertex2D::IndexType*>(indexBuffer.buffer->contents());
				indexBuffer.size = InitialIndexBufferSize;
			}
		}
	}

	void MetalVertexBufferManager2D::updateContent()
	{
		dispatch_semaphore_wait(m_frameBoundarySemaphore, DISPATCH_TIME_FOREVER);
			
		++m_bufferIndex %= MaxInflightBuffers;
		
		m_buffers[m_bufferIndex].vertexBuffer.writePos	= 0;
		m_buffers[m_bufferIndex].indexBuffer.writePos	= 0;
	}

	Vertex2DBufferPointer MetalVertexBufferManager2D::requestBuffer(const uint16 vertexCount, const uint32 indexCount)
	{
		return m_buffers[m_bufferIndex].requestBuffer(vertexCount, indexCount);
	}

	Vertex2DBufferPointer MetalVertexBufferManager2D::Buffer::requestBuffer(const uint16 vertexCount, const uint32 indexCount)
	{
		// VB
		{
			const uint32 vertexArrayWritePosTarget = (vertexBuffer.writePos + vertexCount);

			if (vertexBuffer.size < vertexArrayWritePosTarget)
			{
				return{ nullptr, 0, 0 };
			}
		}
		
		// IB
		{
			const uint32 indexArrayWritePosTarget = (indexBuffer.writePos + indexCount);

			if (indexBuffer.size < indexArrayWritePosTarget)
			{
				return{ nullptr, 0, 0 };
			}
		}
		
		const Vertex2DBufferPointer result
		{
			.pVertex		= (vertexBuffer.pointer	+ vertexBuffer.writePos),
			.pIndex			= (indexBuffer.pointer	+ indexBuffer.writePos),
			.indexOffset	= static_cast<Vertex2D::IndexType>(vertexBuffer.writePos),
		};
		
		vertexBuffer.writePos	+= vertexCount;
		indexBuffer.writePos	+= indexCount;
		
		return result;
	}
}
