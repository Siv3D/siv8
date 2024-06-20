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

# include "D3D11Renderer2DCommandManager.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	D3D11Renderer2DCommandManager::D3D11Renderer2DCommandManager()
	{
		reset();
	}

	////////////////////////////////////////////////////////////////
	//
	//	reset
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::reset()
	{
		// clear commands
		{
			m_commands.clear();
			m_stateTracker.clear();
		}

		// clear buffers
		{
			m_buffer.draws.clear();
			//m_nullDraws.clear();
			m_buffer.colorMuls = { m_buffer.colorMuls.back() };
			m_buffer.colorAdds = { m_buffer.colorAdds.back() };
			m_buffer.blendStates = { m_buffer.blendStates.back() };
			//m_rasterizerStates = { m_rasterizerStates.back() };

			//for (uint32 i = 0; i < SamplerState::MaxSamplerCount; ++i)
			//{
			//	m_vsSamplerStates[i] = { m_vsSamplerStates[i].back() };
			//}

			//for (uint32 i = 0; i < SamplerState::MaxSamplerCount; ++i)
			//{
			//	m_psSamplerStates[i] = { m_psSamplerStates[i].back() };
			//}

			//m_scissorRects = { m_scissorRects.back() };
			//m_viewports = { m_viewports.back() };
			//m_sdfParams = { m_sdfParams.back() };
			//m_internalPSConstants = { m_internalPSConstants.back() };
			//m_RTs = { m_RTs.back() };

			m_buffer.vertexShaders	= { VertexShader::IDType::Invalid() };
			m_buffer.pixelShaders	= { PixelShader::IDType::Invalid() };
			//m_combinedTransforms = { m_combinedTransforms.back() };
			//m_constants.clear();
			//m_constantBufferCommands.clear();
		}

		// clear reserves
		{
			//m_reservedVSs.clear();
			//m_reservedPSs.clear();
			//m_reservedTextures.clear();
		}

		// Begin a new frame
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::SetBuffers, 0);
			m_commands.emplace_back(D3D11Renderer2DCommandType::UpdateBuffers, 0);

			m_commands.emplace_back(D3D11Renderer2DCommandType::ColorMul, 0);
			m_current.colorMul = m_buffer.colorMuls.front();

			m_commands.emplace_back(D3D11Renderer2DCommandType::ColorAdd, 0);
			m_current.colorAdd = m_buffer.colorAdds.front();

			m_commands.emplace_back(D3D11Renderer2DCommandType::BlendState, 0);
			m_current.blendState = m_buffer.blendStates.front();

			//m_commands.emplace_back(D3D11Renderer2DCommandType::RasterizerState, 0);
			//m_currentRasterizerState = m_rasterizerStates.front();

			//for (uint32 i = 0; i < SamplerState::MaxSamplerCount; ++i)
			//{
			//	const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::VSSamplerState0) + i);
			//	m_commands.emplace_back(command, 0);
			//	m_currentVSSamplerStates[i] = m_currentVSSamplerStates.front();
			//}

			//for (uint32 i = 0; i < SamplerState::MaxSamplerCount; ++i)
			//{
			//	const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::PSSamplerState0) + i);
			//	m_commands.emplace_back(command, 0);
			//	m_currentPSSamplerStates[i] = m_currentPSSamplerStates.front();
			//}

			//m_commands.emplace_back(D3D11Renderer2DCommandType::ScissorRect, 0);
			//m_currentScissorRect = m_scissorRects.front();

			//m_commands.emplace_back(D3D11Renderer2DCommandType::Viewport, 0);
			//m_currentViewport = m_viewports.front();

			//m_commands.emplace_back(D3D11Renderer2DCommandType::SDFParams, 0);
			//m_currentSDFParams = m_sdfParams.front();

			//m_commands.emplace_back(D3D11Renderer2DCommandType::InternalPSConstants, 0);
			//m_currentInternalPSConstants = m_internalPSConstants.front();

			//m_commands.emplace_back(D3D11Renderer2DCommandType::SetRT, 0);
			//m_currentRT = m_RTs.front();

			m_commands.emplace_back(D3D11Renderer2DCommandType::SetVS, 0);
			m_current.vertexShader = VertexShader::IDType::Invalid();

			m_commands.emplace_back(D3D11Renderer2DCommandType::SetPS, 0);
			m_current.pixelShader = PixelShader::IDType::Invalid();

			//m_commands.emplace_back(D3D11Renderer2DCommandType::Transform, 0);
			//m_currentCombinedTransform = m_combinedTransforms.front();

			//{
			//	for (uint32 i = 0; i < SamplerState::MaxSamplerCount; ++i)
			//	{
			//		const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::VSTexture0) + i);
			//		m_vsTextures[i] = { Texture::IDType::InvalidValue() };
			//		m_commands.emplace_back(command, 0);
			//	}
			//	m_currentVSTextures.fill(Texture::IDType::InvalidValue());
			//}

			//{
			//	for (uint32 i = 0; i < SamplerState::MaxSamplerCount; ++i)
			//	{
			//		const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::PSTexture0) + i);
			//		m_psTextures[i] = { Texture::IDType::InvalidValue() };
			//		m_commands.emplace_back(command, 0);
			//	}
			//	m_currentPSTextures.fill(Texture::IDType::InvalidValue());
			//}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	flush
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::flush()
	{
		if (m_current.draw.indexCount)
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::Draw, static_cast<uint32>(m_buffer.draws.size()));
			m_buffer.draws.push_back(m_current.draw);
			m_current.draw.indexCount = 0;
		}

		if (m_stateTracker.has(D3D11Renderer2DCommandType::SetBuffers))
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::SetBuffers, 0);
		}

		if (m_stateTracker.has(D3D11Renderer2DCommandType::ColorMul))
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::ColorMul, static_cast<uint32>(m_buffer.colorMuls.size()));
			m_buffer.colorMuls.push_back(m_current.colorMul);
		}

		if (m_stateTracker.has(D3D11Renderer2DCommandType::ColorAdd))
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::ColorAdd, static_cast<uint32>(m_buffer.colorAdds.size()));
			m_buffer.colorAdds.push_back(m_current.colorAdd);
		}

		if (m_stateTracker.has(D3D11Renderer2DCommandType::BlendState))
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::BlendState, static_cast<uint32>(m_buffer.blendStates.size()));
			m_buffer.blendStates.push_back(m_current.blendState);
		}

		//if (m_changes.has(D3D11Renderer2DCommandType::RasterizerState))
		//{
		//	m_commands.emplace_back(D3D11Renderer2DCommandType::RasterizerState, static_cast<uint32>(m_rasterizerStates.size()));
		//	m_rasterizerStates.push_back(m_currentRasterizerState);
		//}

		//for (uint32 i = 0; i < SamplerState::MaxSamplerCount; ++i)
		//{
		//	const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::VSSamplerState0) + i);

		//	if (m_changes.has(command))
		//	{
		//		m_commands.emplace_back(command, static_cast<uint32>(m_vsSamplerStates[i].size()));
		//		m_vsSamplerStates[i].push_back(m_currentVSSamplerStates[i]);
		//	}
		//}

		//for (uint32 i = 0; i < SamplerState::MaxSamplerCount; ++i)
		//{
		//	const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::PSSamplerState0) + i);

		//	if (m_changes.has(command))
		//	{
		//		m_commands.emplace_back(command, static_cast<uint32>(m_psSamplerStates[i].size()));
		//		m_psSamplerStates[i].push_back(m_currentPSSamplerStates[i]);
		//	}
		//}

		//if (m_changes.has(D3D11Renderer2DCommandType::ScissorRect))
		//{
		//	m_commands.emplace_back(D3D11Renderer2DCommandType::ScissorRect, static_cast<uint32>(m_scissorRects.size()));
		//	m_scissorRects.push_back(m_currentScissorRect);
		//}

		//if (m_changes.has(D3D11Renderer2DCommandType::Viewport))
		//{
		//	m_commands.emplace_back(D3D11Renderer2DCommandType::Viewport, static_cast<uint32>(m_viewports.size()));
		//	m_viewports.push_back(m_currentViewport);
		//}

		//if (m_changes.has(D3D11Renderer2DCommandType::SDFParams))
		//{
		//	m_commands.emplace_back(D3D11Renderer2DCommandType::SDFParams, static_cast<uint32>(m_sdfParams.size()));
		//	m_sdfParams.push_back(m_currentSDFParams);
		//}

		//if (m_changes.has(D3D11Renderer2DCommandType::InternalPSConstants))
		//{
		//	m_commands.emplace_back(D3D11Renderer2DCommandType::InternalPSConstants, static_cast<uint32>(m_internalPSConstants.size()));
		//	m_internalPSConstants.push_back(m_currentInternalPSConstants);
		//}

		//if (m_changes.has(D3D11Renderer2DCommandType::SetRT))
		//{
		//	m_commands.emplace_back(D3D11Renderer2DCommandType::SetRT, static_cast<uint32>(m_RTs.size()));
		//	m_RTs.push_back(m_currentRT);
		//}

		if (m_stateTracker.has(D3D11Renderer2DCommandType::SetVS))
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::SetVS, static_cast<uint32>(m_buffer.vertexShaders.size()));
			m_buffer.vertexShaders.push_back(m_current.vertexShader);
		}

		if (m_stateTracker.has(D3D11Renderer2DCommandType::SetPS))
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::SetPS, static_cast<uint32>(m_buffer.pixelShaders.size()));
			m_buffer.pixelShaders.push_back(m_current.pixelShader);
		}

		//if (m_changes.has(D3D11Renderer2DCommandType::Transform))
		//{
		//	m_commands.emplace_back(D3D11Renderer2DCommandType::Transform, static_cast<uint32>(m_combinedTransforms.size()));
		//	m_combinedTransforms.push_back(m_currentCombinedTransform);
		//}

		//if (m_changes.has(D3D11Renderer2DCommandType::SetConstantBuffer))
		//{
		//	assert(not m_constantBufferCommands.isEmpty());
		//	m_commands.emplace_back(D3D11Renderer2DCommandType::SetConstantBuffer, static_cast<uint32>(m_constantBufferCommands.size()) - 1);
		//}

		//for (uint32 i = 0; i < SamplerState::MaxSamplerCount; ++i)
		//{
		//	const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::VSTexture0) + i);

		//	if (m_changes.has(command))
		//	{
		//		m_commands.emplace_back(command, static_cast<uint32>(m_vsTextures[i].size()));
		//		m_vsTextures[i].push_back(m_currentVSTextures[i]);
		//	}
		//}

		//for (int32 i = 0; i < SamplerState::MaxSamplerCount; ++i)
		//{
		//	const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::PSTexture0) + i);

		//	if (m_changes.has(command))
		//	{
		//		m_commands.emplace_back(command, static_cast<uint32>(m_psTextures[i].size()));
		//		m_psTextures[i].push_back(m_currentPSTextures[i]);
		//	}
		//}

		m_stateTracker.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCommands
	//
	////////////////////////////////////////////////////////////////

	const Array<D3D11Renderer2DCommand>& D3D11Renderer2DCommandManager::getCommands() const noexcept
	{
		return m_commands;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushUpdateBuffers
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushUpdateBuffers(const uint32 batchIndex)
	{
		flush();

		m_commands.emplace_back(D3D11Renderer2DCommandType::UpdateBuffers, batchIndex);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushDraw
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushDraw(const Vertex2D::IndexType indexCount)
	{
		if (m_stateTracker.hasStateChange())
		{
			flush();
		}

		m_current.draw.indexCount += indexCount;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getDraw
	//
	////////////////////////////////////////////////////////////////

	const D3D11DrawCommand& D3D11Renderer2DCommandManager::getDraw(const uint32 index) const noexcept
	{
		return m_buffer.draws[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushColorMul
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushColorMul(const Float4& color)
	{
		constexpr auto Command = D3D11Renderer2DCommandType::ColorMul;
		auto& current = m_current.colorMul;
		auto& buffer = m_buffer.colorMuls;

		if (not m_stateTracker.has(Command))
		{
			if (color != current)
			{
				current = color;
				m_stateTracker.set(Command);
			}
		}
		else
		{
			if (color == buffer.back())
			{
				m_stateTracker.clear(Command);
			}

			current = color;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getColorMul
	//
	////////////////////////////////////////////////////////////////

	const Float4& D3D11Renderer2DCommandManager::getColorMul(const uint32 index) const
	{
		return m_buffer.colorMuls[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCurrentColorMul
	//
	////////////////////////////////////////////////////////////////

	const Float4& D3D11Renderer2DCommandManager::getCurrentColorMul() const
	{
		return m_current.colorMul;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushColorAdd
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushColorAdd(const Float3& color)
	{
		constexpr auto Command = D3D11Renderer2DCommandType::ColorAdd;
		auto& current = m_current.colorAdd;
		auto& buffer = m_buffer.colorAdds;

		if (not m_stateTracker.has(Command))
		{
			if (color != current)
			{
				current = color;
				m_stateTracker.set(Command);
			}
		}
		else
		{
			if (color == buffer.back())
			{
				m_stateTracker.clear(Command);
			}

			current = color;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getColorAdd
	//
	////////////////////////////////////////////////////////////////

	const Float3& D3D11Renderer2DCommandManager::getColorAdd(const uint32 index) const
	{
		return m_buffer.colorAdds[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCurrentColorAdd
	//
	////////////////////////////////////////////////////////////////

	const Float3& D3D11Renderer2DCommandManager::getCurrentColorAdd() const
	{
		return m_current.colorAdd;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushBlendState
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushBlendState(const BlendState& state)
	{
		constexpr auto Command = D3D11Renderer2DCommandType::BlendState;
		auto& current = m_current.blendState;
		auto& buffer = m_buffer.blendStates;

		if (not m_stateTracker.has(Command))
		{
			if (state != current)
			{
				current = state;
				m_stateTracker.set(Command);
			}
		}
		else
		{
			if (state == buffer.back())
			{
				m_stateTracker.clear(Command);
			}

			current = state;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBlendState
	//
	////////////////////////////////////////////////////////////////

	const BlendState& D3D11Renderer2DCommandManager::getBlendState(const uint32 index) const
	{
		return m_buffer.blendStates[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCurrentBlendState
	//
	////////////////////////////////////////////////////////////////

	const BlendState& D3D11Renderer2DCommandManager::getCurrentBlendState() const
	{
		return m_current.blendState;
	}





	////////////////////////////////////////////////////////////////
	//
	//	pushEngineVS
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushEngineVS(const VertexShader::IDType id)
	{
		constexpr auto Command = D3D11Renderer2DCommandType::SetVS;
		auto& current = m_current.vertexShader;
		auto& buffer = m_buffer.vertexShaders;

		if (not m_stateTracker.has(Command))
		{
			if (id != current)
			{
				current = id;
				m_stateTracker.set(Command);
			}
		}
		else
		{
			if (id == buffer.back())
			{
				m_stateTracker.clear(Command);
			}
		
			current = id;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getVS
	//
	////////////////////////////////////////////////////////////////

	VertexShader::IDType D3D11Renderer2DCommandManager::getVS(const uint32 index) const
	{
		return m_buffer.vertexShaders[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushEnginePS
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushEnginePS(const PixelShader::IDType id)
	{
		constexpr auto Command = D3D11Renderer2DCommandType::SetPS;
		auto& current = m_current.pixelShader;
		auto& buffer = m_buffer.pixelShaders;

		if (not m_stateTracker.has(Command))
		{
			if (id != current)
			{
				current = id;
				m_stateTracker.set(Command);
			}
		}
		else
		{
			if (id == buffer.back())
			{
				current = id;
				m_stateTracker.clear(Command);
			}
			else
			{
				current = id;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPS
	//
	////////////////////////////////////////////////////////////////

	PixelShader::IDType D3D11Renderer2DCommandManager::getPS(const uint32 index) const
	{
		return m_buffer.pixelShaders[index];
	}
}
