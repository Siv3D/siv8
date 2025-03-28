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

# include "MetalRenderer2DCommandManager.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static float CalculateMaxScaling(const Mat3x2& mat)
		{
			return (Float2{ (mat._11 + mat._21), (mat._12 + mat._22) }.length() / Math::Sqrt2_v<float>);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	MetalRenderer2DCommandManager::MetalRenderer2DCommandManager()
	{
		reset();
	}

	////////////////////////////////////////////////////////////////
	//
	//	reset
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::reset()
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
			m_buffer.quadWarpParameters	= { m_buffer.quadWarpParameters.back() };
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
			//m_sdfParams = { m_sdfParams.back() };
			//m_internalPSConstants = { m_internalPSConstants.back() };
			//m_RTs = { m_RTs.back() };

			m_buffer.vertexShaders		= { VertexShader::IDType::Invalid() };
			m_buffer.pixelShaders		= { PixelShader::IDType::Invalid() };
			m_buffer.combinedTransforms	= { m_buffer.combinedTransforms.back() };
			//m_constants.clear();
			//m_constantBufferCommands.clear();
		}

		// clear reserves
		{
			//m_reservedVSs.clear();
			//m_reservedPSs.clear();
			m_reserved.textures.clear();
		}

		// Begin a new frame
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::ColorMul, 0);
			m_current.colorMul = m_buffer.colorMuls.front();

			m_commands.emplace_back(MetalRenderer2DCommandType::ColorAdd, 0);
			m_current.colorAdd = m_buffer.colorAdds.front();

			m_commands.emplace_back(MetalRenderer2DCommandType::QuadWarpParameters, 0);
			m_current.quadWarpParameter = m_buffer.quadWarpParameters.front();

			m_commands.emplace_back(MetalRenderer2DCommandType::PatternParameters, 0);
			m_current.patternParameter = m_buffer.patternParameters.front();

			m_commands.emplace_back(MetalRenderer2DCommandType::BlendState, 0);
			m_current.blendState = m_buffer.blendStates.front();

			m_commands.emplace_back(MetalRenderer2DCommandType::RasterizerState, 0);
			m_current.rasterizerState = m_buffer.rasterizerStates.front();

			for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
			{
				const auto command = ToEnum<MetalRenderer2DCommandType>(FromEnum(MetalRenderer2DCommandType::VSSamplerState0) + i);
				m_commands.emplace_back(command, 0);
				m_current.vsSamplerStates[i] = m_buffer.vsSamplerStates[i].front();
			}

			for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
			{
				const auto command = ToEnum<MetalRenderer2DCommandType>(FromEnum(MetalRenderer2DCommandType::PSSamplerState0) + i);
				m_commands.emplace_back(command, 0);
				m_current.psSamplerStates[i] = m_buffer.psSamplerStates[i].front();
			}

			m_commands.emplace_back(MetalRenderer2DCommandType::ScissorRect, 0);
			m_current.scissorRect = m_buffer.scissorRects.front();

			m_commands.emplace_back(MetalRenderer2DCommandType::Viewport, 0);
			m_current.viewport = m_buffer.viewports.front();

			//m_commands.emplace_back(D3D11Renderer2DCommandType::SDFParams, 0);
			//m_currentSDFParams = m_sdfParams.front();

			//m_commands.emplace_back(D3D11Renderer2DCommandType::InternalPSConstants, 0);
			//m_currentInternalPSConstants = m_internalPSConstants.front();

			//m_commands.emplace_back(D3D11Renderer2DCommandType::SetRT, 0);
			//m_currentRT = m_RTs.front();

			m_commands.emplace_back(MetalRenderer2DCommandType::SetVS, 0);
			m_current.vertexShader = VertexShader::IDType::Invalid();

			m_commands.emplace_back(MetalRenderer2DCommandType::SetPS, 0);
			m_current.pixelShader = PixelShader::IDType::Invalid();

			m_commands.emplace_back(MetalRenderer2DCommandType::Transform, 0);
			m_current.combinedTransform = m_buffer.combinedTransforms.front();

			{
				for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
				{
					const auto command = ToEnum<MetalRenderer2DCommandType>(FromEnum(MetalRenderer2DCommandType::VSTexture0) + i);
					m_buffer.vsTextures[i] = { Texture::IDType::Invalid() };
					m_commands.emplace_back(command, 0);
				}

				m_current.vsTextures.fill(Texture::IDType::Invalid());
			}

			{
				for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
				{
					const auto command = ToEnum<MetalRenderer2DCommandType>(FromEnum(MetalRenderer2DCommandType::PSTexture0) + i);
					m_buffer.psTextures[i] = { Texture::IDType::Invalid() };
					m_commands.emplace_back(command, 0);
				}
				
				m_current.psTextures.fill(Texture::IDType::Invalid());
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	flush
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::flush()
	{
		if (m_current.draw.indexCount)
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::Draw, static_cast<uint32>(m_buffer.draws.size()));
			m_buffer.draws.push_back(m_current.draw);
			m_current.draw.indexCount = 0;
		}

		if (m_stateTracker.has(MetalRenderer2DCommandType::ColorMul))
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::ColorMul, static_cast<uint32>(m_buffer.colorMuls.size()));
			m_buffer.colorMuls.push_back(m_current.colorMul);
		}

		if (m_stateTracker.has(MetalRenderer2DCommandType::ColorAdd))
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::ColorAdd, static_cast<uint32>(m_buffer.colorAdds.size()));
			m_buffer.colorAdds.push_back(m_current.colorAdd);
		}
		
		if (m_stateTracker.has(MetalRenderer2DCommandType::QuadWarpParameters))
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::QuadWarpParameters, static_cast<uint32>(m_buffer.quadWarpParameters.size()));
			m_buffer.quadWarpParameters.push_back(m_current.quadWarpParameter);
		}

		if (m_stateTracker.has(MetalRenderer2DCommandType::PatternParameters))
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::PatternParameters, static_cast<uint32>(m_buffer.patternParameters.size()));
			m_buffer.patternParameters.push_back(m_current.patternParameter);
		}

		if (m_stateTracker.has(MetalRenderer2DCommandType::BlendState))
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::BlendState, static_cast<uint32>(m_buffer.blendStates.size()));
			m_buffer.blendStates.push_back(m_current.blendState);
		}

		if (m_stateTracker.has(MetalRenderer2DCommandType::RasterizerState))
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::RasterizerState, static_cast<uint32>(m_buffer.rasterizerStates.size()));
			m_buffer.rasterizerStates.push_back(m_current.rasterizerState);
		}

		for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
		{
			const auto command = ToEnum<MetalRenderer2DCommandType>(FromEnum(MetalRenderer2DCommandType::VSSamplerState0) + i);

			if (m_stateTracker.has(command))
			{
				m_commands.emplace_back(command, static_cast<uint32>(m_buffer.vsSamplerStates[i].size()));
				m_buffer.vsSamplerStates[i].push_back(m_current.vsSamplerStates[i]);
			}
		}

		for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
		{
			const auto command = ToEnum<MetalRenderer2DCommandType>(FromEnum(MetalRenderer2DCommandType::PSSamplerState0) + i);
			if (m_stateTracker.has(command))
			{
				m_commands.emplace_back(command, static_cast<uint32>(m_buffer.psSamplerStates[i].size()));
				m_buffer.psSamplerStates[i].push_back(m_current.psSamplerStates[i]);
			}
		}

		if (m_stateTracker.has(MetalRenderer2DCommandType::ScissorRect))
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::ScissorRect, static_cast<uint32>(m_buffer.scissorRects.size()));
			m_buffer.scissorRects.push_back(m_current.scissorRect);
		}

		if (m_stateTracker.has(MetalRenderer2DCommandType::Viewport))
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::Viewport, static_cast<uint32>(m_buffer.viewports.size()));
			m_buffer.viewports.push_back(m_current.viewport);
		}

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

		if (m_stateTracker.has(MetalRenderer2DCommandType::SetVS))
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::SetVS, static_cast<uint32>(m_buffer.vertexShaders.size()));
			m_buffer.vertexShaders.push_back(m_current.vertexShader);
		}

		if (m_stateTracker.has(MetalRenderer2DCommandType::SetPS))
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::SetPS, static_cast<uint32>(m_buffer.pixelShaders.size()));
			m_buffer.pixelShaders.push_back(m_current.pixelShader);
		}

		if (m_stateTracker.has(MetalRenderer2DCommandType::Transform))
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::Transform, static_cast<uint32>(m_buffer.combinedTransforms.size()));
			m_buffer.combinedTransforms.push_back(m_current.combinedTransform);
		}

		//if (m_changes.has(MetalRenderer2DCommandType::SetConstantBuffer))
		//{
		//	assert(not m_constantBufferCommands.isEmpty());
		//	m_commands.emplace_back(D3D11Renderer2DCommandType::SetConstantBuffer, static_cast<uint32>(m_constantBufferCommands.size()) - 1);
		//}

		for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
		{
			const auto command = ToEnum<MetalRenderer2DCommandType>(FromEnum(MetalRenderer2DCommandType::VSTexture0) + i);

			if (m_stateTracker.has(command))
			{
				m_commands.emplace_back(command, static_cast<uint32>(m_buffer.vsTextures[i].size()));
				m_buffer.vsTextures[i].push_back(m_current.vsTextures[i]);
			}
		}

		for (uint32 i = 0; i < Graphics::TextureSlotCount; ++i)
		{
			const auto command = ToEnum<MetalRenderer2DCommandType>(FromEnum(MetalRenderer2DCommandType::PSTexture0) + i);
			
			if (m_stateTracker.has(command))
			{
				m_commands.emplace_back(command, static_cast<uint32>(m_buffer.psTextures[i].size()));
				m_buffer.psTextures[i].push_back(m_current.psTextures[i]);
			}
		}

		m_stateTracker.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCommands
	//
	////////////////////////////////////////////////////////////////

	const Array<MetalRenderer2DCommand>& MetalRenderer2DCommandManager::getCommands() const noexcept
	{
		return m_commands;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushDraw, getDraw
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushDraw(const Vertex2D::IndexType indexCount)
	{
		if (m_stateTracker.hasStateChange())
		{
			flush();
		}

		m_current.draw.indexCount += indexCount;
	}

	const MetalDrawCommand& MetalRenderer2DCommandManager::getDraw(const uint32 index) const noexcept
	{
		return m_buffer.draws[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushColorMul, getColorMul, getCurrentColorMul
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushColorMul(const Float4& color)
	{
		constexpr auto Command = MetalRenderer2DCommandType::ColorMul;
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

	const Float4& MetalRenderer2DCommandManager::getColorMul(const uint32 index) const
	{
		return m_buffer.colorMuls[index];
	}

	const Float4& MetalRenderer2DCommandManager::getCurrentColorMul() const
	{
		return m_current.colorMul;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushColorAdd, getColorAdd, getCurrentColorAdd
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushColorAdd(const Float3& color)
	{
		constexpr auto Command = MetalRenderer2DCommandType::ColorAdd;
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

	const Float3& MetalRenderer2DCommandManager::getColorAdd(const uint32 index) const
	{
		return m_buffer.colorAdds[index];
	}

	const Float3& MetalRenderer2DCommandManager::getCurrentColorAdd() const
	{
		return m_current.colorAdd;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushQuadWarpParameter, getQuadWarpParameter, getQuadWarpParameter
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushQuadWarpParameter(const std::array<Float4, 3>& params)
	{
		constexpr auto Command = MetalRenderer2DCommandType::QuadWarpParameters;
		auto& current = m_current.quadWarpParameter;
		auto& buffer = m_buffer.quadWarpParameters;
		
		if (not m_stateTracker.has(Command))
		{
			if (params != current)
			{
				current = params;
				m_stateTracker.set(Command);
			}
		}
		else
		{
			if (params == buffer.back())
			{
				m_stateTracker.clear(Command);
			}

			current = params;
		}
	}

	const std::array<Float4, 3>& MetalRenderer2DCommandManager::getQuadWarpParameter(const uint32 index) const
	{
		return m_buffer.quadWarpParameters[index];
	}
	
	const std::array<Float4, 3>& MetalRenderer2DCommandManager::getQuadWarpParameter() const
	{
		return m_current.quadWarpParameter;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushPatternParameter, getPatternParameter, getPatternParameter
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushPatternParameter(const std::array<Float4, 3>& patternParameter)
	{
		constexpr auto Command = MetalRenderer2DCommandType::PatternParameters;
		auto& current = m_current.patternParameter;
		auto& buffer = m_buffer.patternParameters;

		if (not m_stateTracker.has(Command))
		{
			if (patternParameter != current)
			{
				current = patternParameter;
				m_stateTracker.set(Command);
			}
		}
		else
		{
			if (patternParameter == buffer.back())
			{
				m_stateTracker.clear(Command);
			}

			current = patternParameter;
		}
	}
	
	const std::array<Float4, 3>& MetalRenderer2DCommandManager::getPatternParameter(const uint32 index) const
	{
		return m_buffer.patternParameters[index];
	}
	
	const std::array<Float4, 3>& MetalRenderer2DCommandManager::getPatternParameter() const
	{
		return m_current.patternParameter;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushBlendState, getBlendState, getCurrentBlendState
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushBlendState(const BlendState& state)
	{
		constexpr auto Command = MetalRenderer2DCommandType::BlendState;
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

	const BlendState& MetalRenderer2DCommandManager::getBlendState(const uint32 index) const
	{
		return m_buffer.blendStates[index];
	}

	const BlendState& MetalRenderer2DCommandManager::getCurrentBlendState() const
	{
		return m_current.blendState;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushRasterizerState, getRasterizerState, getCurrentRasterizerState
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushRasterizerState(const RasterizerState& state)
	{
		constexpr auto Command = MetalRenderer2DCommandType::RasterizerState;
		auto& current = m_current.rasterizerState;
		auto& buffer = m_buffer.rasterizerStates;

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

	const RasterizerState& MetalRenderer2DCommandManager::getRasterizerState(const uint32 index) const
	{
		return m_buffer.rasterizerStates[index];
	}

	const RasterizerState& MetalRenderer2DCommandManager::getCurrentRasterizerState() const
	{
		return m_current.rasterizerState;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushVSSamplerState, getVSSamplerState, getVSCurrentSamplerState
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushVSSamplerState(const SamplerState& state, const uint32 slot)
	{
		assert(slot < Graphics::TextureSlotCount);

		const auto command = ToEnum<MetalRenderer2DCommandType>(FromEnum(MetalRenderer2DCommandType::VSSamplerState0) + slot);
		auto& current = m_current.vsSamplerStates[slot];
		auto& buffer = m_buffer.vsSamplerStates[slot];

		if (not m_stateTracker.has(command))
		{
			if (state != current)
			{
				current = state;
				m_stateTracker.set(command);
			}
		}
		else
		{
			if (state == buffer.back())
			{
				m_stateTracker.clear(command);
			}

			current = state;
		}
	}
	
	const SamplerState& MetalRenderer2DCommandManager::getVSSamplerState(const uint32 slot, const uint32 index) const
	{
		assert(slot < Graphics::TextureSlotCount);

		return m_buffer.vsSamplerStates[slot][index];
	}
	
	const SamplerState& MetalRenderer2DCommandManager::getCurrentVSSamplerState(const uint32 slot) const
	{
		assert(slot < Graphics::TextureSlotCount);

		return m_current.vsSamplerStates[slot];
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushPSSamplerState, getPSSamplerState, getPSCurrentSamplerState
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushPSSamplerState(const SamplerState& state, const uint32 slot)
	{
		assert(slot < Graphics::TextureSlotCount);
		
		const auto command = ToEnum<MetalRenderer2DCommandType>(FromEnum(MetalRenderer2DCommandType::PSSamplerState0) + slot);
		auto& current = m_current.psSamplerStates[slot];
		auto& buffer = m_buffer.psSamplerStates[slot];
		
		if (not m_stateTracker.has(command))
		{
			if (state != current)
			{
				current = state;
				m_stateTracker.set(command);
			}
		}
		else
		{
			if (state == buffer.back())
			{
				m_stateTracker.clear(command);
			}
			
			current = state;
		}
	}

	const SamplerState& MetalRenderer2DCommandManager::getPSSamplerState(const uint32 slot, const uint32 index) const
	{
		assert(slot < Graphics::TextureSlotCount);

		return m_buffer.psSamplerStates[slot][index];
	}

	const SamplerState& MetalRenderer2DCommandManager::getCurrentPSSamplerState(const uint32 slot) const
	{
		assert(slot < Graphics::TextureSlotCount);

		return m_current.psSamplerStates[slot];
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushScissorRect, getScissorRect, getCurrentScissorRect
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushScissorRect(const Optional<Rect>& state)
	{
		constexpr auto Command = MetalRenderer2DCommandType::ScissorRect;
		auto& current = m_current.scissorRect;
		auto& buffer = m_buffer.scissorRects;

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

	const Optional<Rect>& MetalRenderer2DCommandManager::getScissorRect(const uint32 index) const
	{
		return m_buffer.scissorRects[index];
	}

	const Optional<Rect>& MetalRenderer2DCommandManager::getCurrentScissorRect() const
	{
		return m_current.scissorRect;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushViewport, getViewport, getCurrentViewport
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushViewport(const Optional<Rect>& state)
	{
		constexpr auto Command = MetalRenderer2DCommandType::Viewport;
		auto& current = m_current.viewport;
		auto& buffer = m_buffer.viewports;

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
	
	const Optional<Rect>& MetalRenderer2DCommandManager::getViewport(const uint32 index) const
	{
		return m_buffer.viewports[index];
	}
	
	const Optional<Rect>& MetalRenderer2DCommandManager::getCurrentViewport() const
	{
		return m_current.viewport;
	}




	////////////////////////////////////////////////////////////////
	//
	//	pushEngineVS, getVS
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushEngineVS(const VertexShader::IDType id)
	{
		constexpr auto Command = MetalRenderer2DCommandType::SetVS;
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

	VertexShader::IDType MetalRenderer2DCommandManager::getVS(const uint32 index) const
	{
		return m_buffer.vertexShaders[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushEnginePS, getPS
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushEnginePS(const PixelShader::IDType id)
	{
		constexpr auto Command = MetalRenderer2DCommandType::SetPS;
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

	PixelShader::IDType MetalRenderer2DCommandManager::getPS(const uint32 index) const
	{
		return m_buffer.pixelShaders[index];
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushLocalTransform, getCurrentLocalTransform
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushLocalTransform(const Mat3x2& local)
	{
		constexpr auto Command = MetalRenderer2DCommandType::Transform;
		auto& currentLocal = m_current.localTransform;
		auto& currentCombined = m_current.combinedTransform;
		auto& buffer = m_buffer.combinedTransforms;
		const Mat3x2 combinedTransform = (local * m_current.cameraTransform);

		if (not m_stateTracker.has(Command))
		{
			if (local != currentLocal)
			{
				currentLocal = local;
				currentCombined = combinedTransform;
				m_current.maxScaling = CalculateMaxScaling(combinedTransform);
				m_stateTracker.set(Command);
			}
		}
		else
		{
			if (combinedTransform == buffer.back())
			{
				m_stateTracker.clear(Command);
			}

			currentLocal = local;
			currentCombined = combinedTransform;
			m_current.maxScaling = CalculateMaxScaling(combinedTransform);
		}
	}

	const Mat3x2& MetalRenderer2DCommandManager::getCurrentLocalTransform() const
	{
		return m_current.localTransform;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushCameraTransform, getCurrentCameraTransform
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushCameraTransform(const Mat3x2& camera)
	{
		constexpr auto Command = MetalRenderer2DCommandType::Transform;
		auto& currentCamera = m_current.cameraTransform;
		auto& currentCombined = m_current.combinedTransform;
		auto& buffer = m_buffer.combinedTransforms;
		const Mat3x2 combinedTransform = (m_current.localTransform * camera);

		if (not m_stateTracker.has(Command))
		{
			if (camera != currentCamera)
			{
				currentCamera = camera;
				currentCombined = combinedTransform;
				m_current.maxScaling = CalculateMaxScaling(combinedTransform);
				m_stateTracker.set(Command);
			}
		}
		else
		{
			if (combinedTransform == buffer.back())
			{
				m_stateTracker.clear(Command);
			}

			currentCamera = camera;
			currentCombined = combinedTransform;
			m_current.maxScaling = CalculateMaxScaling(combinedTransform);
		}
	}

	const Mat3x2& MetalRenderer2DCommandManager::getCurrentCameraTransform() const
	{
		return m_current.cameraTransform;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCombinedTransform, getCurrentCombinedTransform
	//
	////////////////////////////////////////////////////////////////

	const Mat3x2& MetalRenderer2DCommandManager::getCombinedTransform(const uint32 index) const
	{
		return m_buffer.combinedTransforms[index];
	}

	const Mat3x2& MetalRenderer2DCommandManager::getCurrentCombinedTransform() const
	{
		return m_current.combinedTransform;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCurrentMaxScaling
	//
	////////////////////////////////////////////////////////////////

	float MetalRenderer2DCommandManager::getCurrentMaxScaling() const noexcept
	{
		return m_current.maxScaling;
	}
	
	////////////////////////////////////////////////////////////////
	//
	//	pushVSTextureUnbind, pushVSTexture, getVSTexture, getCurrentVSTextures
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushVSTextureUnbind(const uint32 slot)
	{
		assert(slot < Graphics::TextureSlotCount);

		static constexpr auto InvalidID = Texture::IDType::Invalid();
		const auto command = ToEnum<MetalRenderer2DCommandType>(FromEnum(MetalRenderer2DCommandType::VSTexture0) + slot);
		auto& current = m_current.vsTextures[slot];
		auto& buffer = m_buffer.vsTextures[slot];

		if (not m_stateTracker.has(command))
		{
			if (InvalidID != current)
			{
				current = InvalidID;
				m_stateTracker.set(command);
			}
		}
		else
		{
			if (InvalidID == buffer.back())
			{
				m_stateTracker.clear(command);
			}

			current = InvalidID;
		}
	}

	void MetalRenderer2DCommandManager::pushVSTexture(const uint32 slot, const Texture& texture)
	{
		assert(slot < Graphics::TextureSlotCount);

		const auto command = ToEnum<MetalRenderer2DCommandType>(FromEnum(MetalRenderer2DCommandType::VSTexture0) + slot);
		auto& current = m_current.vsTextures[slot];
		auto& buffer = m_buffer.vsTextures[slot];

		if (not m_stateTracker.has(command))
		{
			if (texture.id() != current)
			{
				current = texture.id();
				m_stateTracker.set(command);
			}
		}
		else
		{
			if (texture.id() == buffer.back())
			{
				m_stateTracker.clear(command);
			}

			current = texture.id();
		}
	}
	
	const Texture::IDType& MetalRenderer2DCommandManager::getVSTexture(const uint32 slot, const uint32 index) const
	{
		assert(slot < Graphics::TextureSlotCount);

		return m_buffer.vsTextures[slot][index];
	}

	const std::array<Texture::IDType, Graphics::TextureSlotCount>& MetalRenderer2DCommandManager::getCurrentVSTextures() const
	{
		return m_current.vsTextures;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pushPSTextureUnbind, pushPSTexture, getPSTexture, getCurrentPSTextures
	//
	////////////////////////////////////////////////////////////////

	void MetalRenderer2DCommandManager::pushPSTextureUnbind(const uint32 slot)
	{
		assert(slot < Graphics::TextureSlotCount);

		static constexpr auto InvalidID = Texture::IDType::Invalid();
		const auto command = ToEnum<MetalRenderer2DCommandType>(FromEnum(MetalRenderer2DCommandType::PSTexture0) + slot);
		auto& current = m_current.psTextures[slot];
		auto& buffer = m_buffer.psTextures[slot];

		if (not m_stateTracker.has(command))
		{
			if (InvalidID != current)
			{
				current = InvalidID;
				m_stateTracker.set(command);
			}
		}
		else
		{
			if (InvalidID == buffer.back())
			{
				m_stateTracker.clear(command);
			}

			current = InvalidID;
		}
	}

	void MetalRenderer2DCommandManager::pushPSTexture(const uint32 slot, const Texture& texture)
	{
		assert(slot < Graphics::TextureSlotCount);

		const auto command = ToEnum<MetalRenderer2DCommandType>(FromEnum(MetalRenderer2DCommandType::PSTexture0) + slot);
		auto& current = m_current.psTextures[slot];
		auto& buffer = m_buffer.psTextures[slot];

		if (not m_stateTracker.has(command))
		{
			if (texture.id() != current)
			{
				current = texture.id();
				m_stateTracker.set(command);
			}
		}
		else
		{
			if (texture.id() == buffer.back())
			{
				m_stateTracker.clear(command);
			}

			current = texture.id();
		}
	}

	const Texture::IDType& MetalRenderer2DCommandManager::getPSTexture(const uint32 slot, const uint32 index) const
	{
		assert(slot < Graphics::TextureSlotCount);

		return m_buffer.psTextures[slot][index];
	}

	const std::array<Texture::IDType, Graphics::TextureSlotCount>& MetalRenderer2DCommandManager::getCurrentPSTextures() const
	{
		return m_current.psTextures;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	std::array<Array<SamplerState>, Graphics::TextureSlotCount> MetalRenderer2DCommandManager::MakeDefaultSamplerStates()
	{
		std::array<Array<SamplerState>, Graphics::TextureSlotCount> result;
		
		result.fill(Array<SamplerState>{ SamplerState::Default2D });
		
		return result;
	}

	std::array<SamplerState, Graphics::TextureSlotCount> MetalRenderer2DCommandManager::MakeDefaultSamplerState()
	{
		std::array<SamplerState, Graphics::TextureSlotCount> result;

		result.fill(SamplerState::Default2D);

		return result;
	}

	std::array<Array<Texture::IDType>, Graphics::TextureSlotCount> MetalRenderer2DCommandManager::MakeDefaultTextures()
	{
		std::array<Array<Texture::IDType>, Graphics::TextureSlotCount> result;
		
		result.fill(Array<Texture::IDType>{ Texture::IDType::Invalid() });
		
		return result;
	}
}
