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
# include <Siv3D/HashTable.hpp>
# include <Siv3D/ConstantBuffer.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Shader/Metal/CShader_Metal.hpp>
# include <Siv3D/Renderer2D/Renderer2DCommon.hpp>
# include "MetalVertexBufferManager2D.hpp"

namespace s3d
{
	struct PipelineStates2D
	{
		VertexShader::IDType vs;

		PixelShader::IDType ps;

		uint16 pixelFormat;

		uint16 sampleCount;

		[[nodiscard]]
		friend constexpr bool operator ==(const PipelineStates2D& lhs, const PipelineStates2D& rhs) noexcept
		{
			return ((lhs.vs == rhs.vs)
				 && (lhs.ps == rhs.ps)
				 && (lhs.pixelFormat == rhs.pixelFormat)
				 && (lhs.sampleCount == rhs.sampleCount));
		}
	};
}

template <>
struct std::hash<s3d::PipelineStates2D>
{
	[[nodiscard]]
	size_t operator ()(const s3d::PipelineStates2D& value) const noexcept
	{
		return s3d::Hash(value);
	}
};

namespace s3d
{
	class CRenderer2D_Metal final : public ISiv3DRenderer2D
	{
	public:

		~CRenderer2D_Metal() override;

		void init() override;

		void beginFrame() override;

		void addTriangle(const Float2(&points)[3], const Float4& color) override;

		void addRect(const FloatRect& rect, const Float4& color) override;

		void flush() override;

		void flush(MTL::CommandBuffer* commandBuffer);

		dispatch_semaphore_t getSemaphore() const;

	private:

		CRenderer_Metal* m_pRenderer	= nullptr;
		
		CShader_Metal* m_pShader		= nullptr;

		MTL::Device* m_device			= nullptr;

		HashTable<PipelineStates2D, NS::SharedPtr<MTL::RenderPipelineState>> m_pipelineStates;
		
		//NS::SharedPtr<MTL::RenderPipelineState> m_pipeLineTestNoAA;
		//NS::SharedPtr<MTL::RenderPipelineState> m_pipeLineTestMSAAx4;

		MetalVertexBufferManager2D m_vertexBufferManager;

		struct EngineShader
		{
			VertexShader::IDType vs;

			PixelShader::IDType psShape;

		} m_engineShader;
	};
}
