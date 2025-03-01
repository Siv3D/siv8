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

# include "D3D11BlendState.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void D3D11BlendState::init(const D3D11Device& device)
	{
		LOG_SCOPED_DEBUG("D3D11BlendState::init()");
		m_device	= device.getDevice();
		m_context	= device.getContext();
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	void D3D11BlendState::set(const BlendState& state, const Float4& blendColor)
	{
		const auto newState = std::make_pair(state, blendColor);

		if (newState == m_currentState)
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

		m_context->OMSetBlendState(it->second.Get(), blendColor.getPointer(), 0xffFFffFF);

		m_currentState = newState;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	D3D11BlendState::StateTable::iterator D3D11BlendState::create(const BlendState& state)
	{
		const D3D11_BLEND_DESC desc
		{
			.AlphaToCoverageEnable	= state.alphaToCoverageEnabled,
			.IndependentBlendEnable	= false,
			.RenderTarget
			{
				{
					.BlendEnable			= state.enabled,
					.SrcBlend				= static_cast<D3D11_BLEND>(state.sourceRGB),
					.DestBlend				= static_cast<D3D11_BLEND>(state.destinationRGB),
					.BlendOp				= static_cast<D3D11_BLEND_OP>(state.rgbOperation),
					.SrcBlendAlpha			= static_cast<D3D11_BLEND>(state.sourceAlpha),
					.DestBlendAlpha			= static_cast<D3D11_BLEND>(state.destinationAlpha),
					.BlendOpAlpha			= static_cast<D3D11_BLEND_OP>(state.alphaOperation),
					.RenderTargetWriteMask	= UINT8((state.writeA << 3) | (state.writeB << 2) | (state.writeG << 1) | uint32(state.writeR)),
				}
			}
		};

		ComPtr<ID3D11BlendState> blendState;

		LOG_TRACE("ID3D11Device::CreateBlendState()");

		if (FAILED(m_device->CreateBlendState(&desc, &blendState)))
		{
			LOG_FAIL("✖ ID3D11Device::CreateBlendState() failed");
			
			return m_states.end();
		}

		return m_states.emplace(state, std::move(blendState)).first;
	}
}
