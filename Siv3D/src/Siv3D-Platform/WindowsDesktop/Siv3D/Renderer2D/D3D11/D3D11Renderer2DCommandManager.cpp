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

# include "D3D11Renderer2DCommandManager.hpp"
# include <Siv3D/Renderer2D/Renderer2DStateUpdate.hpp>
# include <Siv3D/EngineLog.hpp>

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
	//	pushConstantBuffer
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushConstantBuffer(const ShaderStage stage, const uint32 slot, const void* data, const size_t size)
	{
		m_constantBuffers.push(stage, slot, data, size);
		if (m_constantBuffers.hasStateChange())
		{
			m_stateTracker.set(D3D11Renderer2DCommandType::SetConstantBuffer);
		}
		else
		{
			m_stateTracker.clear(D3D11Renderer2DCommandType::SetConstantBuffer);
		}
	}


	uint32 D3D11Renderer2DCommandManager::beginConstantBufferScope(const ShaderStage stage, const uint32 slot, const void* data, const size_t size)
	{
		const uint32 previous = m_constantBuffers.save(stage, slot);
		try
		{
			pushConstantBuffer(stage, slot, data, size);
		}
		catch (...)
		{
			m_constantBuffers.release(previous);
			throw;
		}
		return previous;
	}

	void D3D11Renderer2DCommandManager::endConstantBufferScope(const ShaderStage stage, const uint32 slot, const uint32 previous)
	{
		m_constantBuffers.restore(stage, slot, previous);
		if (m_constantBuffers.hasStateChange())
		{
			m_stateTracker.set(D3D11Renderer2DCommandType::SetConstantBuffer);
		}
		else
		{
			m_stateTracker.clear(D3D11Renderer2DCommandType::SetConstantBuffer);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	reset
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::reset()
	{
		m_constantBuffers.reset();
		// clear commands
		{
			m_commands.clear();
			m_stateTracker.clear();
			m_pendingBatchIndex.reset();
			m_current.draw = {};
		}

		// clear buffers
		{
			m_buffer.draws.clear();
			//m_nullDraws.clear();
			m_buffer.colorMuls = { m_buffer.colorMuls.back() };
			m_buffer.colorAdds = { m_buffer.colorAdds.back() };
			m_buffer.patternParameters	= { m_buffer.patternParameters.back() };
			m_buffer.blendStates = { m_buffer.blendStates.back() };
			m_buffer.rasterizerStates = { m_buffer.rasterizerStates.back() };

			for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
			{
				m_buffer.vsSamplerStates[i] = { m_buffer.vsSamplerStates[i].back() };
			}

			for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
			{
				m_buffer.psSamplerStates[i] = { m_buffer.psSamplerStates[i].back() };
			}

			m_buffer.scissorRects	= { m_buffer.scissorRects.back() };
			m_buffer.viewports		= { m_buffer.viewports.back() };
			m_buffer.sdfParameters	= { m_buffer.sdfParameters.back() };
			//m_internalPSConstants = { m_internalPSConstants.back() };
			//m_RTs = { m_RTs.back() };

			m_buffer.vertexShaders		= { VertexShader::IDType::Invalid() };
			m_buffer.pixelShaders		= { PixelShader::IDType::Invalid() };
			m_buffer.combinedTransforms = { m_buffer.combinedTransforms.back() };
		}

		// clear reserves
		{
			m_reserved.textures.clear();
			m_reserved.vertexShaders.clear();
			m_reserved.pixelShaders.clear();
		}

		// Begin a new frame
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::SetBuffers, 0);
			m_commands.emplace_back(D3D11Renderer2DCommandType::UpdateBuffers, 0);

			m_commands.emplace_back(D3D11Renderer2DCommandType::ColorMul, 0);
			m_current.colorMul = m_buffer.colorMuls.front();

			m_commands.emplace_back(D3D11Renderer2DCommandType::ColorAdd, 0);
			m_current.colorAdd = m_buffer.colorAdds.front();

			m_commands.emplace_back(D3D11Renderer2DCommandType::PatternParameters, 0);
			m_current.patternParameter = m_buffer.patternParameters.front();

			m_commands.emplace_back(D3D11Renderer2DCommandType::BlendState, 0);
			m_current.blendState = m_buffer.blendStates.front();

			m_commands.emplace_back(D3D11Renderer2DCommandType::RasterizerState, 0);
			m_current.rasterizerState = m_buffer.rasterizerStates.front();

			for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
			{
				const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::VSSamplerState0) + i);
				m_commands.emplace_back(command, 0);
				m_current.vsSamplerStates[i] = m_buffer.vsSamplerStates[i].front();
			}

			for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
			{
				const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::PSSamplerState0) + i);
				m_commands.emplace_back(command, 0);
				m_current.psSamplerStates[i] = m_buffer.psSamplerStates[i].front();
			}

			m_commands.emplace_back(D3D11Renderer2DCommandType::ScissorRect, 0);
			m_current.scissorRect = m_buffer.scissorRects.front();

			m_commands.emplace_back(D3D11Renderer2DCommandType::Viewport, 0);
			m_current.viewport = m_buffer.viewports.front();

			m_commands.emplace_back(D3D11Renderer2DCommandType::SDFParams, 0);
			m_current.sdfParameter = m_buffer.sdfParameters.front();

			//m_commands.emplace_back(D3D11Renderer2DCommandType::InternalPSConstants, 0);
			//m_currentInternalPSConstants = m_internalPSConstants.front();

			//m_commands.emplace_back(D3D11Renderer2DCommandType::SetRT, 0);
			//m_currentRT = m_RTs.front();

			m_commands.emplace_back(D3D11Renderer2DCommandType::SetVS, 0);
			m_current.vertexShader = VertexShader::IDType::Invalid();

			m_commands.emplace_back(D3D11Renderer2DCommandType::SetPS, 0);
			m_current.pixelShader = PixelShader::IDType::Invalid();

			m_commands.emplace_back(D3D11Renderer2DCommandType::Transform, 0);
			m_current.transform.combinedTransform = m_buffer.combinedTransforms.front();

			{
				for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
				{
					const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::VSTexture0) + i);
					m_buffer.vsTextures[i] = { Texture::IDType::Invalid() };
					m_commands.emplace_back(command, 0);
				}

				m_current.vsTextures.fill(Texture::IDType::Invalid());
			}

			{
				for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
				{
					const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::PSTexture0) + i);
					m_buffer.psTextures[i] = { Texture::IDType::Invalid() };
					m_commands.emplace_back(command, 0);
				}
				
				m_current.psTextures.fill(Texture::IDType::Invalid());
			}
		}

		for (uint32 index = 0; index < m_constantBuffers.size(); ++index)
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::SetConstantBuffer, index);
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
			m_current.draw.startIndex += m_current.draw.indexCount;
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

		if (m_stateTracker.has(D3D11Renderer2DCommandType::PatternParameters))
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::PatternParameters, static_cast<uint32>(m_buffer.patternParameters.size()));
			m_buffer.patternParameters.push_back(m_current.patternParameter);
		}

		if (m_stateTracker.has(D3D11Renderer2DCommandType::BlendState))
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::BlendState, static_cast<uint32>(m_buffer.blendStates.size()));
			m_buffer.blendStates.push_back(m_current.blendState);
		}

		if (m_stateTracker.has(D3D11Renderer2DCommandType::RasterizerState))
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::RasterizerState, static_cast<uint32>(m_buffer.rasterizerStates.size()));
			m_buffer.rasterizerStates.push_back(m_current.rasterizerState);
		}

		for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
		{
			const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::VSSamplerState0) + i);

			if (m_stateTracker.has(command))
			{
				m_commands.emplace_back(command, static_cast<uint32>(m_buffer.vsSamplerStates[i].size()));
				m_buffer.vsSamplerStates[i].push_back(m_current.vsSamplerStates[i]);
			}
		}

		for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
		{
			const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::PSSamplerState0) + i);
			if (m_stateTracker.has(command))
			{
				m_commands.emplace_back(command, static_cast<uint32>(m_buffer.psSamplerStates[i].size()));
				m_buffer.psSamplerStates[i].push_back(m_current.psSamplerStates[i]);
			}
		}

		if (m_stateTracker.has(D3D11Renderer2DCommandType::ScissorRect))
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::ScissorRect, static_cast<uint32>(m_buffer.scissorRects.size()));
			m_buffer.scissorRects.push_back(m_current.scissorRect);
		}

		if (m_stateTracker.has(D3D11Renderer2DCommandType::Viewport))
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::Viewport, static_cast<uint32>(m_buffer.viewports.size()));
			m_buffer.viewports.push_back(m_current.viewport);
		}

		if (m_stateTracker.has(D3D11Renderer2DCommandType::SDFParams))
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::SDFParams, static_cast<uint32>(m_buffer.sdfParameters.size()));
			m_buffer.sdfParameters.push_back(m_current.sdfParameter);
		}

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

		if (m_stateTracker.has(D3D11Renderer2DCommandType::Transform))
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::Transform, static_cast<uint32>(m_buffer.combinedTransforms.size()));
			m_buffer.combinedTransforms.push_back(m_current.transform.combinedTransform);
		}

		for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
		{
			const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::VSTexture0) + i);

			if (m_stateTracker.has(command))
			{
				m_commands.emplace_back(command, static_cast<uint32>(m_buffer.vsTextures[i].size()));
				m_buffer.vsTextures[i].push_back(m_current.vsTextures[i]);
			}
		}

		for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
		{
			const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::PSTexture0) + i);
			
			if (m_stateTracker.has(command))
			{
				m_commands.emplace_back(command, static_cast<uint32>(m_buffer.psTextures[i].size()));
				m_buffer.psTextures[i].push_back(m_current.psTextures[i]);
			}
		}

		if (m_stateTracker.has(D3D11Renderer2DCommandType::SetConstantBuffer))
		{
			const uint32 first = static_cast<uint32>(m_constantBuffers.size());
			m_constantBuffers.flush();
			for (uint32 index = first; index < m_constantBuffers.size(); ++index)
			{
				m_commands.emplace_back(D3D11Renderer2DCommandType::SetConstantBuffer, index);
			}
		}

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
	//	deferUpdateBuffers
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::deferUpdateBuffers(const uint32 batchIndex)
	{
		// 確保だけでは描画されるとは限らないため、切り替えは次の pushDraw() まで保留する。
		// その間に別のバッチへ進んだ場合、中間のバッチには Draw がないので転送を省略できる。
		m_pendingBatchIndex = batchIndex;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushDraw, getDraw
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushDraw(const uint32 indexCount)
	{
		// 描画状態またはバッチを切り替える前に、それまでの Draw を確定する。
		if (m_stateTracker.hasStateChange() || m_pendingBatchIndex)
		{
			flush();
		}

		if (m_pendingBatchIndex)
		{
			m_commands.emplace_back(D3D11Renderer2DCommandType::UpdateBuffers, *m_pendingBatchIndex);
			m_current.draw.startIndex = 0;
			m_pendingBatchIndex.reset();
		}

		// 各 Build は図形全体を一度に確保するため、今回のインデックスはすべて同じバッチに属する。
		m_current.draw.indexCount += indexCount;
	}

	const D3D11DrawCommand& D3D11Renderer2DCommandManager::getDraw(const uint32 index) const noexcept
	{
		return m_buffer.draws[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushColorMul, getColorMul, getCurrentColorMul
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushColorMul(const Float4& color)
	{
		UpdateRenderer2DState(m_stateTracker, D3D11Renderer2DCommandType::ColorMul, color,
			m_current.colorMul, m_buffer.colorMuls);
	}

	const Float4& D3D11Renderer2DCommandManager::getColorMul(const uint32 index) const
	{
		return m_buffer.colorMuls[index];
	}

	const Float4& D3D11Renderer2DCommandManager::getCurrentColorMul() const
	{
		return m_current.colorMul;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushColorAdd, getColorAdd, getCurrentColorAdd
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushColorAdd(const Float3& color)
	{
		UpdateRenderer2DState(m_stateTracker, D3D11Renderer2DCommandType::ColorAdd, color,
			m_current.colorAdd, m_buffer.colorAdds);
	}

	const Float3& D3D11Renderer2DCommandManager::getColorAdd(const uint32 index) const
	{
		return m_buffer.colorAdds[index];
	}

	const Float3& D3D11Renderer2DCommandManager::getCurrentColorAdd() const
	{
		return m_current.colorAdd;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushPatternParameter, getPatternParameter, getPatternParameter
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushPatternParameter(const std::array<Float4, 4>& patternParameter)
	{
		UpdateRenderer2DState(m_stateTracker, D3D11Renderer2DCommandType::PatternParameters, patternParameter,
			m_current.patternParameter, m_buffer.patternParameters);
	}
	
	const std::array<Float4, 4>& D3D11Renderer2DCommandManager::getPatternParameter(const uint32 index) const
	{
		return m_buffer.patternParameters[index];
	}
	
	const std::array<Float4, 4>& D3D11Renderer2DCommandManager::getPatternParameter() const
	{
		return m_current.patternParameter;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushBlendState, getBlendState, getCurrentBlendState
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushBlendState(const BlendState& state)
	{
		UpdateRenderer2DState(m_stateTracker, D3D11Renderer2DCommandType::BlendState, state,
			m_current.blendState, m_buffer.blendStates);
	}

	const BlendState& D3D11Renderer2DCommandManager::getBlendState(const uint32 index) const
	{
		return m_buffer.blendStates[index];
	}

	const BlendState& D3D11Renderer2DCommandManager::getCurrentBlendState() const
	{
		return m_current.blendState;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushRasterizerState, getRasterizerState, getCurrentRasterizerState
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushRasterizerState(const RasterizerState& state)
	{
		UpdateRenderer2DState(m_stateTracker, D3D11Renderer2DCommandType::RasterizerState, state,
			m_current.rasterizerState, m_buffer.rasterizerStates);
	}

	const RasterizerState& D3D11Renderer2DCommandManager::getRasterizerState(const uint32 index) const
	{
		return m_buffer.rasterizerStates[index];
	}

	const RasterizerState& D3D11Renderer2DCommandManager::getCurrentRasterizerState() const
	{
		return m_current.rasterizerState;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushVSSamplerState, getVSSamplerState, getVSCurrentSamplerState
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushVSSamplerState(const SamplerState& state, const uint32 slot)
	{
		assert(slot < Graphics::TextureSlotCount);

		const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::VSSamplerState0) + slot);
		UpdateRenderer2DState(m_stateTracker, command, state,
			m_current.vsSamplerStates[slot], m_buffer.vsSamplerStates[slot]);
	}
	
	const SamplerState& D3D11Renderer2DCommandManager::getVSSamplerState(const uint32 slot, const uint32 index) const
	{
		assert(slot < Graphics::TextureSlotCount);

		return m_buffer.vsSamplerStates[slot][index];
	}
	
	const SamplerState& D3D11Renderer2DCommandManager::getCurrentVSSamplerState(const uint32 slot) const
	{
		assert(slot < Graphics::TextureSlotCount);

		return m_current.vsSamplerStates[slot];
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushPSSamplerState, getPSSamplerState, getPSCurrentSamplerState
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushPSSamplerState(const SamplerState& state, const uint32 slot)
	{
		assert(slot < Graphics::TextureSlotCount);

		const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::PSSamplerState0) + slot);
		UpdateRenderer2DState(m_stateTracker, command, state,
			m_current.psSamplerStates[slot], m_buffer.psSamplerStates[slot]);
	}

	const SamplerState& D3D11Renderer2DCommandManager::getPSSamplerState(const uint32 slot, const uint32 index) const
	{
		assert(slot < Graphics::TextureSlotCount);

		return m_buffer.psSamplerStates[slot][index];
	}

	const SamplerState& D3D11Renderer2DCommandManager::getCurrentPSSamplerState(const uint32 slot) const
	{
		assert(slot < Graphics::TextureSlotCount);

		return m_current.psSamplerStates[slot];
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushScissorRect, getScissorRect, getCurrentScissorRect
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushScissorRect(const Optional<Rect>& state)
	{
		UpdateRenderer2DState(m_stateTracker, D3D11Renderer2DCommandType::ScissorRect, state,
			m_current.scissorRect, m_buffer.scissorRects);
	}

	const Optional<Rect>& D3D11Renderer2DCommandManager::getScissorRect(const uint32 index) const
	{
		return m_buffer.scissorRects[index];
	}

	const Optional<Rect>& D3D11Renderer2DCommandManager::getCurrentScissorRect() const
	{
		return m_current.scissorRect;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushViewport, getViewport, getCurrentViewport
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushViewport(const Optional<Rect>& state)
	{
		UpdateRenderer2DState(m_stateTracker, D3D11Renderer2DCommandType::Viewport, state,
			m_current.viewport, m_buffer.viewports);
	}
	
	const Optional<Rect>& D3D11Renderer2DCommandManager::getViewport(const uint32 index) const
	{
		return m_buffer.viewports[index];
	}
	
	const Optional<Rect>& D3D11Renderer2DCommandManager::getCurrentViewport() const
	{
		return m_current.viewport;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushSDFParameters, getSDFParameters, getCurrentSDFParameters
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushSDFParameters(const std::array<Float4, 3>& state)
	{
		UpdateRenderer2DState(m_stateTracker, D3D11Renderer2DCommandType::SDFParams, state,
			m_current.sdfParameter, m_buffer.sdfParameters);
	}

	const std::array<Float4, 3>& D3D11Renderer2DCommandManager::getSDFParameters(const uint32 index) const
	{
		return m_buffer.sdfParameters[index];
	}
	
	const std::array<Float4, 3>& D3D11Renderer2DCommandManager::getCurrentSDFParameters() const
	{
		return m_current.sdfParameter;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushEngineVS, pushCustomVS, getVS
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushEngineVS(const VertexShader::IDType id)
	{
		UpdateRenderer2DState(m_stateTracker, D3D11Renderer2DCommandType::SetVS, id,
			m_current.vertexShader, m_buffer.vertexShaders);
	}

	void D3D11Renderer2DCommandManager::pushCustomVS(const VertexShader& vs)
	{
		UpdateRenderer2DCustomShaderState(m_stateTracker, D3D11Renderer2DCommandType::SetVS, vs,
			m_current.vertexShader, m_buffer.vertexShaders, m_reserved.vertexShaders);
	}

	VertexShader::IDType D3D11Renderer2DCommandManager::getVS(const uint32 index) const
	{
		return m_buffer.vertexShaders[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushEnginePS, pushCustomPS, getPS
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushEnginePS(const PixelShader::IDType id)
	{
		UpdateRenderer2DState(m_stateTracker, D3D11Renderer2DCommandType::SetPS, id,
			m_current.pixelShader, m_buffer.pixelShaders);
	}

	void D3D11Renderer2DCommandManager::pushCustomPS(const PixelShader& ps)
	{
		UpdateRenderer2DCustomShaderState(m_stateTracker, D3D11Renderer2DCommandType::SetPS, ps,
			m_current.pixelShader, m_buffer.pixelShaders, m_reserved.pixelShaders);
	}

	PixelShader::IDType D3D11Renderer2DCommandManager::getPS(const uint32 index) const
	{
		return m_buffer.pixelShaders[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushLocalTransform, getCurrentLocalTransform
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushLocalTransform(const Mat3x2& local)
	{
		if (local == m_current.transform.localTransform)
		{
			return;
		}
		m_current.transform.localTransform = local;
		updateAffineTransform();
	}

	const Mat3x2& D3D11Renderer2DCommandManager::getCurrentLocalTransform() const
	{
		return m_current.transform.localTransform;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushCameraTransform, getCurrentCameraTransform
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushCameraTransform(const Mat3x2& camera)
	{
		if (camera == m_current.transform.cameraTransform)
		{
			return;
		}
		m_current.transform.cameraTransform = camera;
		updateAffineTransform();
	}

	const Mat3x2& D3D11Renderer2DCommandManager::getCurrentCameraTransform() const
	{
		return m_current.transform.cameraTransform;
	}

	////////////////////////////////////////////////////////////////
	//
	//	QuadWarp and combined transforms
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushQuadWarpTransform(const Mat3x3& warp)
	{
		if (warp == m_current.transform.quadWarpTransform)
		{
			return;
		}
		m_current.transform.quadWarpTransform = warp;
		updateTransform();
	}

	const Mat3x3& D3D11Renderer2DCommandManager::getCurrentQuadWarpTransform() const
	{
		return m_current.transform.quadWarpTransform;
	}

	void D3D11Renderer2DCommandManager::setSceneScale(const Float2 scale)
	{
		if (scale == m_current.transform.sceneScale)
		{
			return;
		}
		m_current.transform.sceneScale = scale;
		updateRasterScaling();
	}

	void D3D11Renderer2DCommandManager::updateRasterScaling()
	{
		m_current.transform.updateRasterScaling();
	}

	void D3D11Renderer2DCommandManager::updateAffineTransform()
	{
		m_current.transform.updateAffineTransform();
		updateRasterScaling();
		updateTransform();
	}

	void D3D11Renderer2DCommandManager::updateTransform()
	{
		constexpr auto Command = D3D11Renderer2DCommandType::Transform;
		m_current.transform.updateCombinedTransform();
		if (m_current.transform.combinedTransform == m_buffer.combinedTransforms.back())
		{
			m_stateTracker.clear(Command);
		}
		else
		{
			m_stateTracker.set(Command);
		}
	}

	const Mat3x3& D3D11Renderer2DCommandManager::getCombinedTransform(const uint32 index) const
	{
		return m_buffer.combinedTransforms[index];
	}

	const Mat3x3& D3D11Renderer2DCommandManager::getCurrentCombinedTransform() const
	{
		return m_current.transform.combinedTransform;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCurrentRMSScaling
	//
	////////////////////////////////////////////////////////////////

	float D3D11Renderer2DCommandManager::getCurrentRMSScaling() const noexcept
	{
		return m_current.transform.rmsScaling;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushVSTextureUnbind, pushVSTexture, getVSTexture, getCurrentVSTextures
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushVSTextureUnbind(const uint32 slot)
	{
		assert(slot < Graphics::TextureSlotCount);

		static constexpr auto InvalidID = Texture::IDType::Invalid();
		const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::VSTexture0) + slot);
		UpdateRenderer2DState(m_stateTracker, command, InvalidID,
			m_current.vsTextures[slot], m_buffer.vsTextures[slot]);
	}

	void D3D11Renderer2DCommandManager::pushVSTexture(const uint32 slot, const Texture& texture)
	{
		assert(slot < Graphics::TextureSlotCount);

		const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::VSTexture0) + slot);
		UpdateRenderer2DTextureState(m_stateTracker, command, texture,
			m_current.vsTextures[slot], m_buffer.vsTextures[slot], m_reserved.textures);
	}
	
	const Texture::IDType& D3D11Renderer2DCommandManager::getVSTexture(const uint32 slot, const uint32 index) const
	{
		assert(slot < Graphics::TextureSlotCount);

		return m_buffer.vsTextures[slot][index];
	}

	const std::array<Texture::IDType, Graphics::TextureSlotCount>& D3D11Renderer2DCommandManager::getCurrentVSTextures() const
	{
		return m_current.vsTextures;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushPSTextureUnbind, pushPSTexture, getPSTexture, getCurrentPSTextures
	//
	////////////////////////////////////////////////////////////////

	void D3D11Renderer2DCommandManager::pushPSTextureUnbind(const uint32 slot)
	{
		assert(slot < Graphics::TextureSlotCount);

		static constexpr auto InvalidID = Texture::IDType::Invalid();
		const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::PSTexture0) + slot);
		UpdateRenderer2DState(m_stateTracker, command, InvalidID,
			m_current.psTextures[slot], m_buffer.psTextures[slot]);
	}

	void D3D11Renderer2DCommandManager::pushPSTexture(const uint32 slot, const Texture& texture)
	{
		assert(slot < Graphics::TextureSlotCount);

		const auto command = ToEnum<D3D11Renderer2DCommandType>(FromEnum(D3D11Renderer2DCommandType::PSTexture0) + slot);
		UpdateRenderer2DTextureState(m_stateTracker, command, texture,
			m_current.psTextures[slot], m_buffer.psTextures[slot], m_reserved.textures);
	}

	const Texture::IDType& D3D11Renderer2DCommandManager::getPSTexture(const uint32 slot, const uint32 index) const
	{
		assert(slot < Graphics::TextureSlotCount);

		return m_buffer.psTextures[slot][index];
	}

	const std::array<Texture::IDType, Graphics::TextureSlotCount>& D3D11Renderer2DCommandManager::getCurrentPSTextures() const
	{
		return m_current.psTextures;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	std::array<Array<SamplerState>, Graphics::TextureSlotCount> D3D11Renderer2DCommandManager::MakeDefaultSamplerStates()
	{
		std::array<Array<SamplerState>, Graphics::TextureSlotCount> result;
		
		result.fill(Array<SamplerState>{ SamplerState::Default2D });
		
		return result;
	}

	std::array<SamplerState, Graphics::TextureSlotCount> D3D11Renderer2DCommandManager::MakeDefaultSamplerState()
	{
		std::array<SamplerState, Graphics::TextureSlotCount> result;

		result.fill(SamplerState::Default2D);

		return result;
	}

	std::array<Array<Texture::IDType>, Graphics::TextureSlotCount> D3D11Renderer2DCommandManager::MakeDefaultTextures()
	{
		std::array<Array<Texture::IDType>, Graphics::TextureSlotCount> result;
		
		result.fill(Array<Texture::IDType>{ Texture::IDType::Invalid() });
		
		return result;
	}
}
