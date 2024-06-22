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

# include "D3D11DepthStencilState.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void D3D11DepthStencilState::init(const D3D11Device& device)
	{
		LOG_SCOPED_DEBUG("D3D11DepthStencilState::init()");
		m_device	= device.getDevice();
		m_context	= device.getContext();
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	void D3D11DepthStencilState::set(const DepthStencilState& state)
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

		m_context->OMSetDepthStencilState(it->second.Get(), 0);

		m_currentState = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	D3D11DepthStencilState::StateTable::iterator D3D11DepthStencilState::create(const DepthStencilState& state)
	{
		const D3D11_DEPTH_STENCIL_DESC desc
		{
			.DepthEnable					= state.depthTestEnabled,
			.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK(state.depthWriteEnabled),
			.DepthFunc						= D3D11_COMPARISON_FUNC(state.depthCompareFunction),
			.StencilEnable					= false,
			.StencilReadMask				= 0xFF,
			.StencilWriteMask				= 0xFF,
			.FrontFace =
				{
					.StencilFailOp		= D3D11_STENCIL_OP_KEEP,
					.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP,
					.StencilPassOp		= D3D11_STENCIL_OP_KEEP,
					.StencilFunc		= D3D11_COMPARISON_ALWAYS,
				},
			.BackFace =
				{
					.StencilFailOp		= D3D11_STENCIL_OP_KEEP,
					.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP,
					.StencilPassOp		= D3D11_STENCIL_OP_KEEP,
					.StencilFunc		= D3D11_COMPARISON_ALWAYS,
				}
		};

		ComPtr<ID3D11DepthStencilState> depthStencilState;

		LOG_TRACE("ID3D11Device::CreateDepthStencilState()");

		if (FAILED(m_device->CreateDepthStencilState(&desc, &depthStencilState)))
		{
			LOG_FAIL("✖ ID3D11Device::CreateDepthStencilState() failed");

			return m_states.end();
		}

		if (4096 <= m_states.size())
		{
			m_states.clear();
		}

		return m_states.emplace(state, std::move(depthStencilState)).first;
	}
}
