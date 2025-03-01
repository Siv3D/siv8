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

# include "D3D11RasterizerState.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void D3D11RasterizerState::init(const D3D11Device& device)
	{
		LOG_SCOPED_DEBUG("D3D11RasterizerState::init()");
		m_device	= device.getDevice();
		m_context	= device.getContext();
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	void D3D11RasterizerState::set(const RasterizerState& state, const bool scissorEnabled)
	{
		const auto newState = std::make_pair(state, scissorEnabled);

		if (newState == m_currentState)
		{
			return;
		}

		auto it = m_states.find(newState);

		if (it == m_states.end())
		{
			it = create(newState);

			if (it == m_states.end())
			{
				return;
			}
		}

		m_context->RSSetState(it->second.Get());

		m_currentState = newState;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setScissorRect
	//
	////////////////////////////////////////////////////////////////

	void D3D11RasterizerState::setScissorRect(const Rect& scissorRect)
	{
		const D3D11_RECT r{ scissorRect.x, scissorRect.y, (scissorRect.x + scissorRect.w), (scissorRect.y + scissorRect.h) };

		m_context->RSSetScissorRects(1, &r);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	D3D11RasterizerState::StateTable::iterator D3D11RasterizerState::create(const std::pair<RasterizerState, bool>& state)
	{
		const D3D11_RASTERIZER_DESC desc
		{
			.FillMode				= static_cast<D3D11_FILL_MODE>(FromEnum(state.first.triangleFillMode) + 2),
			.CullMode				= static_cast<D3D11_CULL_MODE>(state.first.cullMode),
			.FrontCounterClockwise	= false,
			.DepthBias				= state.first.depthBias,
			.DepthBiasClamp			= 0.0f,
			.SlopeScaledDepthBias	= 0.0f,
			.DepthClipEnable		= true,
			.ScissorEnable			= state.second,
			.MultisampleEnable		= (state.first.antialiasedLine3D ? false : true),
			.AntialiasedLineEnable	= (state.first.antialiasedLine3D ? true : false),
		};

		ComPtr<ID3D11RasterizerState> rasterizerState;

		LOG_TRACE("ID3D11Device::CreateRasterizerState()");

		if (FAILED(m_device->CreateRasterizerState(&desc, &rasterizerState)))
		{
			LOG_FAIL("✖ ID3D11Device::CreateRasterizerState() failed");

			return m_states.end();
		}

		if (4096 <= m_states.size())
		{
			m_states.clear();
		}

		return m_states.emplace(state, std::move(rasterizerState)).first;
	}
}
