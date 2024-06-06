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
		for (auto& vertexBuffer: m_vertexBuffers)
		{
			vertexBuffer.buffer = NS::TransferPtr(device->newBuffer((sizeof(Vertex2D) * vertexBuffer.size), MTL::ResourceStorageModeShared));
			vertexBuffer.pointer = static_cast<Vertex2D*>(vertexBuffer.buffer->contents());
		}
		
		for (auto& indexBuffer: m_indexBuffers)
		{
			indexBuffer.buffer = NS::TransferPtr(device->newBuffer((sizeof(Vertex2D::IndexType) * indexBuffer.size), MTL::ResourceStorageModeShared));
			indexBuffer.pointer = static_cast<Vertex2D::IndexType*>(indexBuffer.buffer->contents());
		}
	}

	void MetalVertexBufferManager2D::updateContent()
	{
		dispatch_semaphore_wait(m_frameBoundarySemaphore, DISPATCH_TIME_FOREVER);
			
		++m_bufferIndex %= MaxInflightBuffers;
		
		m_vertexBuffers[m_bufferIndex].writePos = 0;
		m_indexBuffers[m_bufferIndex].writePos = 0;
	}
}
