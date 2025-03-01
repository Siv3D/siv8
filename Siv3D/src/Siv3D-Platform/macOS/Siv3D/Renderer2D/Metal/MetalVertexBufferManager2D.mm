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

# include "MetalVertexBufferManager2D.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	void MetalVertexBufferManager2D::init(MTL::Device* device)
	{
		m_device = device;
		
		for (uint32 bufferIndex = 0; auto& buffer: m_buffers)
		{
			{
				auto& vertexBuffer = buffer.vertexBuffer;
				vertexBuffer.bufferIndex = bufferIndex;
				vertexBuffer.buffer = NS::TransferPtr(device->newBuffer((sizeof(Vertex2D) * InitialVertexBufferSize), MTL::ResourceStorageModeShared));
				vertexBuffer.pointer = static_cast<Vertex2D*>(vertexBuffer.buffer->contents());
				vertexBuffer.size = InitialVertexBufferSize;
			}
			
			{
				auto& indexBuffer = buffer.indexBuffer;
				indexBuffer.bufferIndex = bufferIndex;
				indexBuffer.buffer = NS::TransferPtr(device->newBuffer((sizeof(Vertex2D::IndexType) * InitialIndexBufferSize), MTL::ResourceStorageModeShared));
				indexBuffer.pointer = static_cast<Vertex2D::IndexType*>(indexBuffer.buffer->contents());
				indexBuffer.size = InitialIndexBufferSize;
			}
			
			++bufferIndex;
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
		return m_buffers[m_bufferIndex].requestBuffer(m_device, vertexCount, indexCount);
	}

	bool MetalVertexBufferManager2D::hasBatch() const noexcept
	{
		return (m_buffers[m_bufferIndex].indexBuffer.writePos != 0);
	}

	uint32 MetalVertexBufferManager2D::indexCount() const noexcept
	{
		return m_buffers[m_bufferIndex].indexBuffer.writePos;
	}

	Vertex2DBufferPointer MetalVertexBufferManager2D::Buffer::requestBuffer(MTL::Device* device, const uint16 vertexCount, const uint32 indexCount)
	{
		// VB
		{
			const uint32 vertexArrayWritePosTarget = (vertexBuffer.writePos + vertexCount);

			if (vertexBuffer.size < vertexArrayWritePosTarget)
			{
				if (MaxVertexBufferSize < vertexArrayWritePosTarget)
				{
					return{ nullptr, 0, 0 };
				}
				
				vertexBuffer.resize(device, vertexArrayWritePosTarget);
			}
		}
		
		// IB
		{
			const uint32 indexArrayWritePosTarget = (indexBuffer.writePos + indexCount);

			if (indexBuffer.size < indexArrayWritePosTarget)
			{
				if (MaxIndexBufferSize < indexArrayWritePosTarget)
				{
					return{ nullptr, 0, 0 };
				}
				
				indexBuffer.resize(device, indexArrayWritePosTarget);
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

	void MetalVertexBufferManager2D::VertexBuffer::resize(MTL::Device* device, const uint32 vertexArrayWritePosTarget)
	{
		const Vertex2D* oldPointer = pointer;
		const uint32 oldSize = size;
			
		const uint32 newVertexArraySize = std::bit_ceil(vertexArrayWritePosTarget);
		NS::SharedPtr<MTL::Buffer> newBuffer = NS::TransferPtr(device->newBuffer((sizeof(Vertex2D) * newVertexArraySize), MTL::ResourceStorageModeShared));
		Vertex2D* newPointer = static_cast<Vertex2D*>(newBuffer->contents());

		std::memcpy(newPointer, oldPointer, (sizeof(Vertex2D) * oldSize));
		
		buffer = newBuffer;
		pointer = newPointer;
		size = newVertexArraySize;

		LOG_TRACE(fmt::format("ℹ️ Resized MetalVertexBufferManager2D::VertexBuffer[{}] (size: {} -> {})", bufferIndex, size, newVertexArraySize));
	}

	void MetalVertexBufferManager2D::IndexBuffer::resize(MTL::Device* device, const uint32 indexArrayWritePosTarget)
	{
		const Vertex2D::IndexType* oldPointer = pointer;
		const uint32 oldSize = size;
			
		const uint32 newIndexArraySize = std::bit_ceil(indexArrayWritePosTarget);
		NS::SharedPtr<MTL::Buffer> newBuffer = NS::TransferPtr(device->newBuffer((sizeof(Vertex2D::IndexType) * newIndexArraySize), MTL::ResourceStorageModeShared));
		Vertex2D::IndexType* newPointer = static_cast<Vertex2D::IndexType*>(newBuffer->contents());
	
		std::memcpy(newPointer, oldPointer, (sizeof(Vertex2D::IndexType) * oldSize));
		
		buffer = newBuffer;
		pointer = newPointer;
		size = newIndexArraySize;

		LOG_TRACE(fmt::format("ℹ️ Resized MetalVertexBufferManager2D::IndexBuffer[{}] (size: {} -> {})", bufferIndex, size, newIndexArraySize));
	}
}
