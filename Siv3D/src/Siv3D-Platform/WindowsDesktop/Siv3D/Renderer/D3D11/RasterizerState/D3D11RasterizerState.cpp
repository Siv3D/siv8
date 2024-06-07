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

	void D3D11RasterizerState::set(const RasterizerState& state)
	{
		if (state == m_currentState)
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

		m_context->RSSetState(it->second.Get());

		m_currentState = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setScissorRect
	//
	////////////////////////////////////////////////////////////////

	void D3D11RasterizerState::setScissorRect(const Rect& scissorRect)
	{
		const D3D11_RECT r{ scissorRect.x, scissorRect.y, scissorRect.x + scissorRect.w, scissorRect.y + scissorRect.h };

		m_context->RSSetScissorRects(1, &r);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	D3D11RasterizerState::StateTable::iterator D3D11RasterizerState::create(const RasterizerState& state)
	{
		const D3D11_RASTERIZER_DESC desc
		{
			.FillMode				= static_cast<D3D11_FILL_MODE>(FromEnum(state.triangleFillMode) + 2),
			.CullMode				= static_cast<D3D11_CULL_MODE>(state.cullMode),
			.FrontCounterClockwise	= false,
			.DepthBias				= state.depthBias,
			.DepthBiasClamp			= 0.0f,
			.SlopeScaledDepthBias	= 0.0f,
			.DepthClipEnable		= true,
			.ScissorEnable			= state.scissorEnabled,
			.MultisampleEnable		= (state.antialiasedLine3D ? false : true),
			.AntialiasedLineEnable	= (state.antialiasedLine3D ? true : false),
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
