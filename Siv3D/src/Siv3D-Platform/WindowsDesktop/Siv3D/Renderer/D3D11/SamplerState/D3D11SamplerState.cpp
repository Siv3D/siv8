//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "D3D11SamplerState.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		static constexpr D3D11_FILTER FilterTable[8] =
		{
			D3D11_FILTER_MIN_MAG_MIP_POINT,
			D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
			D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
			D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,
			D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,
			D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
			D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
			D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		};

		static constexpr D3D11_TEXTURE_ADDRESS_MODE AddressModeTable[6] =
		{
			D3D11_TEXTURE_ADDRESS_MODE(0),
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_MIRROR,
			D3D11_TEXTURE_ADDRESS_MIRROR_ONCE,
			D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_TEXTURE_ADDRESS_BORDER,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void D3D11SamplerState::init(const D3D11Device& device)
	{
		LOG_SCOPED_DEBUG("D3D11SamplerState::init()");
		m_device	= device.getDevice();
		m_context	= device.getContext();
		m_currentVSStates.fill(SamplerState::Invalid());
		m_currentPSStates.fill(SamplerState::Invalid());
	}

	////////////////////////////////////////////////////////////////
	//
	//	setVS
	//
	////////////////////////////////////////////////////////////////

	void D3D11SamplerState::setVS(const uint32 slot, const SamplerState& state)
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

		m_context->VSSetSamplers(slot, 1, it->second.GetAddressOf());

		m_currentVSStates[slot] = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPS
	//
	////////////////////////////////////////////////////////////////

	void D3D11SamplerState::setPS(const uint32 slot, const SamplerState& state)
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

		m_context->PSSetSamplers(slot, 1, it->second.GetAddressOf());

		m_currentPSStates[slot] = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	D3D11SamplerState::StateTable::iterator D3D11SamplerState::create(const SamplerState& state)
	{
		const uint32 filterTableIndex =
			  ((FromEnum(state.minFilter) & 1) << 2)
			| ((FromEnum(state.magFilter) & 1) << 1)
			| (FromEnum(state.mipFilter) & 1);

		D3D11_SAMPLER_DESC desc
		{
			.Filter			= FilterTable[filterTableIndex],
			.AddressU		= AddressModeTable[FromEnum(state.uAddressMode)],
			.AddressV		= AddressModeTable[FromEnum(state.vAddressMode)],
			.AddressW		= AddressModeTable[FromEnum(state.wAddressMode)],
			.MipLODBias		= 0.0f,
			.MaxAnisotropy	= state.maxAnisotropy,
			.ComparisonFunc	= D3D11_COMPARISON_FUNC(FromEnum(state.compareFunction)),
			.BorderColor	= { 0.0f, 0.0f, 0.0f, 0.0f },
			.MinLOD			= state.minLOD,
			.MaxLOD			= D3D11_FLOAT32_MAX,
		};

		if (state.borderColor == SampleBorderColor::OpaqueBlack)
		{
			desc.BorderColor[3] = 1.0f;
		}
		else if (state.borderColor == SampleBorderColor::OpaqueWhite)
		{
			desc.BorderColor[0] = 1.0f;
			desc.BorderColor[1] = 1.0f;
			desc.BorderColor[2] = 1.0f;
			desc.BorderColor[3] = 1.0f;
		}

		if ((desc.Filter == D3D11_FILTER_MIN_MAG_MIP_LINEAR)
			&& (1 < desc.MaxAnisotropy))
		{
			desc.Filter = D3D11_FILTER_ANISOTROPIC;
		}

		ComPtr<ID3D11SamplerState> samplerState;

		LOG_TRACE("ID3D11Device::CreateSamplerState()");

		if (FAILED(m_device->CreateSamplerState(&desc, &samplerState)))
		{
			LOG_FAIL("✖ ID3D11Device::CreateSamplerState() failed");

			return m_states.end();
		}

		if (4096 <= m_states.size())
		{
			m_states.clear();
		}

		return m_states.emplace(state, std::move(samplerState)).first;
	}
}
