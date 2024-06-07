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
	namespace
	{
		static constexpr std::array<MTL::BlendFactor, 20> BlendFactorTable
		{
			MTL::BlendFactor{ 0 },
			MTL::BlendFactorZero,
			MTL::BlendFactorOne,
			MTL::BlendFactorSourceColor,
			MTL::BlendFactorOneMinusSourceColor,
			MTL::BlendFactorSourceAlpha,
			MTL::BlendFactorOneMinusSourceAlpha,
			MTL::BlendFactorDestinationAlpha,
			MTL::BlendFactorOneMinusDestinationAlpha,
			MTL::BlendFactorDestinationColor,
			MTL::BlendFactorOneMinusDestinationColor,
			MTL::BlendFactorSourceAlphaSaturated,
			MTL::BlendFactor{ 0 },
			MTL::BlendFactor{ 0 },
			MTL::BlendFactorBlendColor,
			MTL::BlendFactorOneMinusBlendColor,
			MTL::BlendFactorSource1Color,
			MTL::BlendFactorOneMinusSource1Color,
			MTL::BlendFactorSource1Alpha,
			MTL::BlendFactorOneMinusSource1Alpha
		};
	
		static constexpr std::array<MTL::BlendOperation, 6> BlendOperationTable
		{
			MTL::BlendOperation{ 0 },
			MTL::BlendOperationAdd,
			MTL::BlendOperationSubtract,
			MTL::BlendOperationReverseSubtract,
			MTL::BlendOperationMin,
			MTL::BlendOperationMax
		};
	
		static void SetBlendState(MTL::RenderPipelineDescriptor* pd, MTL::RenderPipelineColorAttachmentDescriptor* cd, const BlendState& state)
		{
			cd->setBlendingEnabled(state.enabled);
			cd->setSourceRGBBlendFactor(BlendFactorTable[FromEnum(state.sourceRGB)]);
			cd->setDestinationRGBBlendFactor(BlendFactorTable[FromEnum(state.destinationRGB)]);
			cd->setRgbBlendOperation(BlendOperationTable[FromEnum(state.rgbOperation)]);
			cd->setSourceAlphaBlendFactor(BlendFactorTable[FromEnum(state.sourceAlpha)]);
			cd->setDestinationAlphaBlendFactor(BlendFactorTable[FromEnum(state.destinationAlpha)]);
			cd->setAlphaBlendOperation(BlendOperationTable[FromEnum(state.alphaOperation)]);
			pd->setAlphaToCoverageEnabled(state.alphaToCoverageEnabled);
			{
				MTL::ColorWriteMask mask = 0;
				
				if (state.writeR)
				{
					mask |= MTL::ColorWriteMaskRed;
				}
				
				if (state.writeG)
				{
					mask |= MTL::ColorWriteMaskGreen;
				}
				
				if (state.writeB)
				{
					mask |= MTL::ColorWriteMaskBlue;
				}
				
				if (state.writeA)
				{
					mask |= MTL::ColorWriteMaskAlpha;
				}
				
				cd->setWriteMask(mask);
			}
		}
	}

	void MetalRenderPipelineState::init(MTL::Device* device, CShader_Metal* pShader)
	{
		LOG_SCOPED_DEBUG("MetalRenderPipelineState::init()");
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
			renderPipelineDescriptor->setSampleCount(desc.sampleCount);
			MTL::RenderPipelineColorAttachmentDescriptor* cd = renderPipelineDescriptor->colorAttachments()->object(0);
			cd->setPixelFormat(ToEnum<MTL::PixelFormat>(desc.pixelFormat));
			SetBlendState(renderPipelineDescriptor.get(), cd, desc.blendState);
			
			NS::Error* error;
			NS::SharedPtr<MTL::RenderPipelineState> pipelineState = NS::TransferPtr(m_device->newRenderPipelineState(renderPipelineDescriptor.get(), &error));
			it = m_pipelineStates.emplace(desc, std::move(pipelineState)).first;
			LOG_DEBUG(fmt::format("Created RenderPipelineState2D({}, {}, {}, {}, {})",
								  desc.vs.value(), desc.ps.value(), desc.pixelFormat, desc.sampleCount, std::hash<BlendState>{}(desc.blendState)));
		}
		
		return it->second.get();
	}
}
