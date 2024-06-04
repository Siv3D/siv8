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

# include "MetalRenderPipelineState.hpp"
# include <Siv3D/Shader/Metal/CShader_Metal.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	void MetalRenderPipelineState::init(MTL::Device* device, CShader_Metal* pShader)
	{
		m_device	= device;
		m_pShader	= pShader;
	}

	const MTL::RenderPipelineState* MetalRenderPipelineState::get(const PipelineStateDesc& desc)
	{
		auto it = m_pipelineStates.find(desc);

		if (it == m_pipelineStates.end())
		{
			NS::SharedPtr<MTL::RenderPipelineDescriptor> renderPipelineDescriptor = NS::TransferPtr(MTL::RenderPipelineDescriptor::alloc()->init());
			renderPipelineDescriptor->setVertexFunction(m_pShader->getShaderVS(desc.vs));
			renderPipelineDescriptor->setFragmentFunction(m_pShader->getShaderPS(desc.ps));
			renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(ToEnum<MTL::PixelFormat>(desc.pixelFormat));
			renderPipelineDescriptor->setSampleCount(desc.sampleCount);
			
			NS::Error* error;
			NS::SharedPtr<MTL::RenderPipelineState> pipelineState = NS::TransferPtr(m_device->newRenderPipelineState(renderPipelineDescriptor.get(), &error));
			it = m_pipelineStates.emplace(desc, std::move(pipelineState)).first;
			LOG_DEBUG(fmt::format("Created RenderPipelineState2D({}, {}, {}, {})", desc.vs.value(), desc.ps.value(), desc.pixelFormat, desc.sampleCount));
		}
		
		return it->second.get();
	}
}
