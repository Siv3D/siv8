//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "D3D11BlendState.hpp"
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>

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

	void D3D11BlendState::set(const BlendState& state, const Float4& blendColor, const uint8 colorWriteMask)
	{
		const auto newState = std::make_pair(state, blendColor);

		if ((newState == m_currentState) && (colorWriteMask == m_currentColorWriteMask))
		{
			return;
		}

		const uint64 key = (uint64(state.asValue()) | (uint64(colorWriteMask) << 32));
		auto it = m_states.find(key);

		if (it == m_states.end())
		{
			it = create(state, colorWriteMask);
		}

		m_context->OMSetBlendState(it->second.Get(), blendColor.getPointer(), 0xffFFffFF);

		m_currentState = newState;
		m_currentColorWriteMask = colorWriteMask;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	D3D11BlendState::StateTable::iterator D3D11BlendState::create(const BlendState& state, const uint8 colorWriteMask)
	{
		D3D11_BLEND_DESC desc
		{
			.AlphaToCoverageEnable	= state.alphaToCoverageEnabled,
			.IndependentBlendEnable	= (colorWriteMask != 0xFF),
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

		for (uint32 i = 1; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			desc.RenderTarget[i] = desc.RenderTarget[0];
		}
		for (uint32 i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			if ((colorWriteMask & (1u << i)) == 0)
			{
				desc.RenderTarget[i].BlendEnable = false;
				desc.RenderTarget[i].RenderTargetWriteMask = 0;
			}
		}

		ComPtr<ID3D11BlendState> blendState;

		LOG_TRACE("ID3D11Device::CreateBlendState()");

		const HRESULT hr = m_device->CreateBlendState(&desc, &blendState);
		if (FAILED(hr))
		{
			throw InternalEngineError{ fmt::format("ID3D11Device::CreateBlendState() failed (blendState = {}, colorWriteMask = {}, HRESULT = {:#x})",
				state.asValue(), colorWriteMask, static_cast<uint32>(hr)) };
		}

		const uint64 key = (uint64(state.asValue()) | (uint64(colorWriteMask) << 32));
		return m_states.emplace(key, std::move(blendState)).first;
	}
}
