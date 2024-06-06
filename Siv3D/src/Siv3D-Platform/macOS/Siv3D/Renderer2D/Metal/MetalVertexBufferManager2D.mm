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
	void MetalVertexBufferManager2D::init()
	{
		
	}

	void MetalVertexBufferManager2D::updateContent()
	{
		dispatch_semaphore_wait(frameBoundarySemaphore, DISPATCH_TIME_FOREVER);
			
		++m_bufferIndex %= MaxInflightBuffers;
	}
}
