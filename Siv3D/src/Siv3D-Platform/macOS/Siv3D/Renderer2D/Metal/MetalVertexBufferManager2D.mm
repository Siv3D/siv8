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
		for (size_t i = 0; i < MaxInflightBuffers; ++i)
		{
			const uint16 testIndices[3] = { 0, 1, 2 };
			
			indexBuffers[i] = NS::TransferPtr(device->newBuffer(&testIndices, sizeof(testIndices), MTL::ResourceStorageModeShared));
		}
	}

	void MetalVertexBufferManager2D::updateContent()
	{
		dispatch_semaphore_wait(m_frameBoundarySemaphore, DISPATCH_TIME_FOREVER);
			
		++m_bufferIndex %= MaxInflightBuffers;
	}
}
