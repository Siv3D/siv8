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
# include <Siv3D/Renderer2D/Vertex2DBuilder.hpp>

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
			return m_buffers[m_bufferIndex].vertexBuffer.buffer.get();
		}

		const MTL::Buffer* getIndexBuffer() const
		{
			return m_buffers[m_bufferIndex].indexBuffer.buffer.get();
		}

		Vertex2DBufferPointer requestBuffer(uint16 vertexCount, uint32 indexCount);
		
		bool hasBatch() const noexcept;
		
		uint32 indexCount() const noexcept;

	private:

		static constexpr uint32 InitialVertexBufferSize	= (1 << 16);	// 65,536

		static constexpr uint32 InitialIndexBufferSize	= (1 << 16);	// 65,536

		static constexpr uint32 MaxVertexBufferSize		= (1 << 22);	// 4,194,304
		
		static constexpr uint32 MaxIndexBufferSize		= (1 << 22);	// 4,194,304
		
		static constexpr uint32 MaxVertexCountPerDraw	= 65535;
		
		struct VertexBuffer
		{
			// triple-buffer のインデックス
			uint32 bufferIndex = 0;
			
			uint32 size = 0;
			
			uint32 writePos = 0;
			
			Vertex2D* pointer = 0;
			
			NS::SharedPtr<MTL::Buffer> buffer;
			
			void resize(MTL::Device* device, uint32 vertexArrayWritePosTarget);
			
			Vertex2D* requestVertexBuffer(uint32 count)
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
			// triple-buffer のインデックス
			uint32 bufferIndex = 0;
			
			uint32 size = 0;
			
			uint32 writePos = 0;
			
			Vertex2D::IndexType* pointer = 0;
			
			NS::SharedPtr<MTL::Buffer> buffer;
			
			void resize(MTL::Device* device, uint32 indexArrayWritePosTarget);
			
			Vertex2D::IndexType* requestIndexBuffer(uint32 count)
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
		
		struct Buffer
		{
			VertexBuffer vertexBuffer;
			
			IndexBuffer indexBuffer;
			
			Vertex2DBufferPointer requestBuffer(MTL::Device* device, uint16 vertexCount, uint32 indexCount);
		};
		
		MTL::Device* m_device = nullptr;
	
		std::array<Buffer, MaxInflightBuffers> m_buffers;
		
		dispatch_semaphore_t m_frameBoundarySemaphore = dispatch_semaphore_create(MaxInflightBuffers);
		
		size_t m_bufferIndex = 0;	
	};
}
