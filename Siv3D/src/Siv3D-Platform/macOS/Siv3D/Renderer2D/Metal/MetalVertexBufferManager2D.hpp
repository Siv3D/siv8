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

# pragma once
# include <Siv3D/Array.hpp>
# include <Siv3D/Vertex2D.hpp>
# include <Siv3D/Renderer/Metal/Metal.hpp>

namespace s3d
{
	class MetalVertexBufferManager2D
	{
	public:
		
		static constexpr size_t MaxInflightBuffers = 3;
		
		dispatch_semaphore_t frameBoundarySemaphore = dispatch_semaphore_create(MaxInflightBuffers);
	
		std::array<NS::SharedPtr<MTL::Buffer>, MaxInflightBuffers> vertexBuffers;
		
		std::array<NS::SharedPtr<MTL::Buffer>, MaxInflightBuffers> indexBuffers;

		void updateContent()
		{
			dispatch_semaphore_wait(frameBoundarySemaphore, DISPATCH_TIME_FOREVER);
			
			++m_bufferIndex %= MaxInflightBuffers;
		}
		
		dispatch_semaphore_t getSemaphore() const
		{
			return frameBoundarySemaphore;
		}
		
		NS::SharedPtr<MTL::Buffer>& getCurrentVertexBuffer()
		{
			return vertexBuffers[m_bufferIndex];
		}
		
	private:
		
		size_t m_bufferIndex = 0;	
	};
}
