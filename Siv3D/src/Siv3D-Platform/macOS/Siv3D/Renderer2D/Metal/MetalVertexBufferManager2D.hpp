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

		std::array<NS::SharedPtr<MTL::Buffer>, MaxInflightBuffers> indexBuffers;
	
		void init(MTL::Device* device);

		void updateContent();
		
		dispatch_semaphore_t getSemaphore() const
		{
			return m_frameBoundarySemaphore;
		}

		const MTL::Buffer* getVertexBuffer() const
		{
			return m_vertexBuffers[m_bufferIndex].buffer.get();
		}

		const MTL::Buffer* getIndexBuffer() const
		{
			return m_indexBuffers[m_bufferIndex].buffer.get();
		}

		Vertex2D* requestVertexBuffer(size_t count)
		{
			return m_vertexBuffers[m_bufferIndex].requestVertexBuffer(count);
		}

		uint16* requestIndexBuffer(size_t count)
		{
			return m_indexBuffers[m_bufferIndex].requestIndexBuffer(count);
		}

	private:

		struct VertexBuffer
		{
			size_t size = 4096;
			
			size_t writePos = 0;
			
			Vertex2D* pointer = 0;
			
			NS::SharedPtr<MTL::Buffer> buffer;
			
			Vertex2D* requestVertexBuffer(size_t count)
			{
				if (size < (writePos + count))
				{
					return nullptr;
				}
				
				Vertex2D* result = (pointer + writePos);
				
				writePos += count;
				
				return result;
			}
		};
	
		struct IndexBuffer
		{
			size_t size = 4096;
			
			size_t writePos = 0;
			
			Vertex2D::IndexType* pointer = 0;
			
			NS::SharedPtr<MTL::Buffer> buffer;
			
			Vertex2D::IndexType* requestIndexBuffer(size_t count)
			{
				if (size < (writePos + count))
				{
					return nullptr;
				}
				
				Vertex2D::IndexType* result = (pointer + writePos);
				
				writePos += count;
				
				return result;
			}
		};
	
		std::array<VertexBuffer, MaxInflightBuffers> m_vertexBuffers;
		
		std::array<IndexBuffer, MaxInflightBuffers> m_indexBuffers;
				
		dispatch_semaphore_t m_frameBoundarySemaphore = dispatch_semaphore_create(MaxInflightBuffers);
		
		size_t m_bufferIndex = 0;	
	};
}
