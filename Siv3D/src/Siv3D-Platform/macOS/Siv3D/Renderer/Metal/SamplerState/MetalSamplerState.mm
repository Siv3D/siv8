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

# include "MetalSamplerState.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		static constexpr MTL::SamplerAddressMode AddressModeTable[6] =
		{
			MTL::SamplerAddressMode{ 0 },
			MTL::SamplerAddressModeRepeat,
			MTL::SamplerAddressModeMirrorRepeat,
			MTL::SamplerAddressModeMirrorClampToEdge,
			MTL::SamplerAddressModeClampToEdge,
			MTL::SamplerAddressModeClampToBorderColor,
		};

		static constexpr MTL::CompareFunction CompareFunctionTable[9] =
		{
			MTL::CompareFunction{ 0 },
			MTL::CompareFunctionNever,
			MTL::CompareFunctionLess,
			MTL::CompareFunctionEqual,
			MTL::CompareFunctionLessEqual,
			MTL::CompareFunctionGreater,
			MTL::CompareFunctionNotEqual,
			MTL::CompareFunctionGreaterEqual,
			MTL::CompareFunctionAlways,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void MetalSamplerState::init(MTL::Device* device)
	{
		LOG_SCOPED_DEBUG("MetalSamplerState::init()");
		m_device = device;
		m_currentVSStates.fill(SamplerState::Invalid());
		m_currentPSStates.fill(SamplerState::Invalid());
	}

	////////////////////////////////////////////////////////////////
	//
	//	setVS
	//
	////////////////////////////////////////////////////////////////

	void MetalSamplerState::setVS(MTL::RenderCommandEncoder* renderCommandEncoder, const uint32 slot, const SamplerState& state)
	{
		assert(slot < Graphics::TextureSlotCount);

		if (state == m_currentVSStates[slot])
		{
			return;
		}

		auto it = m_states.find(state);

		if (it == m_states.end())
		{
			it = create(state);

			if (it == m_states.end())
			{
				return;
			}
		}
		
		renderCommandEncoder->setVertexSamplerState(it->second.get(), slot);

		m_currentVSStates[slot] = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPS
	//
	////////////////////////////////////////////////////////////////

	void MetalSamplerState::setPS(MTL::RenderCommandEncoder* renderCommandEncoder, const uint32 slot, const SamplerState& state)
	{
		assert(slot < Graphics::TextureSlotCount);

		if (state == m_currentPSStates[slot])
		{
			return;
		}

		auto it = m_states.find(state);

		if (it == m_states.end())
		{
			it = create(state);

			if (it == m_states.end())
			{
				return;
			}
		}
		
		renderCommandEncoder->setFragmentSamplerState(it->second.get(), slot);

		m_currentPSStates[slot] = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	MetalSamplerState::StateTable::iterator MetalSamplerState::create(const SamplerState& state)
	{
		NS::SharedPtr<MTL::SamplerDescriptor> samplerDescriptor = NS::TransferPtr(MTL::SamplerDescriptor::alloc()->init());
		samplerDescriptor->setSAddressMode(AddressModeTable[FromEnum(state.uAddressMode)]);
		samplerDescriptor->setTAddressMode(AddressModeTable[FromEnum(state.vAddressMode)]);
		samplerDescriptor->setRAddressMode(AddressModeTable[FromEnum(state.wAddressMode)]);
		samplerDescriptor->setMagFilter((state.magFilter == TextureFilter::Linear) ? MTL::SamplerMinMagFilterLinear : MTL::SamplerMinMagFilterNearest);
		samplerDescriptor->setMinFilter((state.minFilter == TextureFilter::Linear) ? MTL::SamplerMinMagFilterLinear : MTL::SamplerMinMagFilterNearest);
		samplerDescriptor->setMipFilter((state.mipFilter == TextureFilter::Linear) ? MTL::SamplerMipFilterLinear : MTL::SamplerMipFilterNearest);
		samplerDescriptor->setMaxAnisotropy(state.maxAnisotropy);
		samplerDescriptor->setCompareFunction(CompareFunctionTable[FromEnum(state.compareFunction)]);

		if (state.borderColor == Float4{ 0.0f, 0.0f, 0.0f, 1.0f })
		{
			samplerDescriptor->setBorderColor(MTL::SamplerBorderColorOpaqueBlack);
		}
		else if (state.borderColor == Float4{ 1.0f, 1.0f, 1.0f, 1.0f })
		{
			samplerDescriptor->setBorderColor(MTL::SamplerBorderColorOpaqueWhite);
		}
		else
		{
			samplerDescriptor->setBorderColor(MTL::SamplerBorderColorTransparentBlack);
		}
		
		samplerDescriptor->setLodMinClamp(state.minLOD);

		LOG_TRACE(U"MTL::Device::newSamplerState()");

		NS::SharedPtr<MTL::SamplerState> samplerState = NS::TransferPtr(m_device->newSamplerState(samplerDescriptor.get()));

		if (not samplerState)
		{
			LOG_FAIL(U"✖ MTL::Device::newSamplerState() failed");
			return m_states.end();
		}
				
		if (4096 <= m_states.size())
		{
			m_states.clear();
		}
		
		return m_states.emplace(state, std::move(samplerState)).first;
	}
}
