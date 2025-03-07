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

# pragma once
# include <Siv3D/Array.hpp>
# include <Siv3D/HashMap.hpp>
# include <Siv3D/Vertex2D.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/BlendState.hpp>
# include <Siv3D/RasterizerState.hpp>
# include <Siv3D/SamplerState.hpp>
# include <Siv3D/VertexShader.hpp>
# include <Siv3D/PixelShader.hpp>
# include <Siv3D/Mat3x2.hpp>
# include <Siv3D/Graphics.hpp>
# include <Siv3D/Texture.hpp>
# include <Siv3D/Renderer/D3D11/D3D11.hpp>
# include "D3D11Renderer2DCommand.hpp"
# include <Siv3D/Renderer2D/BatchStateTracker.hpp>

namespace s3d
{
	class D3D11Renderer2DCommandManager
	{
	public:

		D3D11Renderer2DCommandManager();

		void reset();

		void flush();

		const Array<D3D11Renderer2DCommand>& getCommands() const noexcept;

		void pushUpdateBuffers(uint32 batchIndex);

		void pushDraw(Vertex2D::IndexType indexCount);
		const D3D11DrawCommand& getDraw(uint32 index) const noexcept;

		void pushColorMul(const Float4& color);
		const Float4& getColorMul(uint32 index) const;
		const Float4& getCurrentColorMul() const;

		void pushColorAdd(const Float3& color);
		const Float3& getColorAdd(uint32 index) const;
		const Float3& getCurrentColorAdd() const;

		void pushQuadWarpParameter(const std::array<Float4, 3>& color);
		const std::array<Float4, 3>& getQuadWarpParameter(uint32 index) const;
		const std::array<Float4, 3>& getQuadWarpParameter() const;

		void pushPatternParameter(const std::array<Float4, 3>& color);
		const std::array<Float4, 3>& getPatternParameter(uint32 index) const;
		const std::array<Float4, 3>& getPatternParameter() const;

		void pushBlendState(const BlendState& state);
		const BlendState& getBlendState(uint32 index) const;
		const BlendState& getCurrentBlendState() const;

		void pushRasterizerState(const RasterizerState& state);
		const RasterizerState& getRasterizerState(uint32 index) const;
		const RasterizerState& getCurrentRasterizerState() const;

		void pushVSSamplerState(const SamplerState& state, uint32 slot);
		const SamplerState& getVSSamplerState(uint32 slot, uint32 index) const;
		const SamplerState& getCurrentVSSamplerState(uint32 slot) const;

		void pushPSSamplerState(const SamplerState& state, uint32 slot);
		const SamplerState& getPSSamplerState(uint32 slot, uint32 index) const;
		const SamplerState& getCurrentPSSamplerState(uint32 slot) const;

		void pushScissorRect(const Optional<Rect>& state);
		const Optional<Rect>& getScissorRect(uint32 index) const;
		const Optional<Rect>& getCurrentScissorRect() const;

		void pushViewport(const Optional<Rect>& state);
		const Optional<Rect>& getViewport(uint32 index) const;
		const Optional<Rect>& getCurrentViewport() const;

		void pushEngineVS(VertexShader::IDType id);
		VertexShader::IDType getVS(uint32 index) const;

		void pushEnginePS(PixelShader::IDType id);
		PixelShader::IDType getPS(uint32 index) const;

		void pushLocalTransform(const Mat3x2& local);
		const Mat3x2& getCurrentLocalTransform() const;

		void pushCameraTransform(const Mat3x2& camera);
		const Mat3x2& getCurrentCameraTransform() const;

		const Mat3x2& getCombinedTransform(uint32 index) const;
		const Mat3x2& getCurrentCombinedTransform() const;
		float getCurrentMaxScaling() const noexcept;

		void pushVSTextureUnbind(uint32 slot);
		void pushVSTexture(uint32 slot, const Texture& texture);
		const Texture::IDType& getVSTexture(uint32 slot, uint32 index) const;
		const std::array<Texture::IDType, Graphics::TextureSlotCount>& getCurrentVSTextures() const;

		void pushPSTextureUnbind(uint32 slot);
		void pushPSTexture(uint32 slot, const Texture& texture);
		const Texture::IDType& getPSTexture(uint32 slot, uint32 index) const;
		const std::array<Texture::IDType, Graphics::TextureSlotCount>& getCurrentPSTextures() const;

	private:

		Array<D3D11Renderer2DCommand> m_commands;

		BatchStateTracker<D3D11Renderer2DCommandType> m_stateTracker;

		struct Buffer
		{
			Array<D3D11DrawCommand> draws;

			Array<Float4> colorMuls					= { Float4{ 1.0f, 1.0f, 1.0f, 1.0f } };
			
			Array<Float3> colorAdds					= { Float3{ 0.0f, 0.0f, 0.0f } };

			Array<std::array<Float4, 3>> quadWarpParameters	= { std::array<Float4, 3>{ Float4{ 0.0f, 0.0f, 1.0f, 0.0f }, Float4{ 1.0f, 1.0f, 0.0f, 1.0f }, Float4{ 0.0f, 0.0f, 0.0f, 0.0f } } };

			Array<std::array<Float4, 3>> patternParameters	= { std::array<Float4, 3>{ Float4{ 0.0f, 0.0f, 0.0f, 0.0f }, Float4{ 0.0f, 0.0f, 0.0f, 0.0f }, Float4{ 0.0f, 0.0f, 0.0f, 0.0f } } };

			Array<BlendState> blendStates			= { BlendState::Default2D };

			Array<RasterizerState> rasterizerStates	= { RasterizerState::Default2D };

			std::array<Array<SamplerState>, Graphics::TextureSlotCount> vsSamplerStates = MakeDefaultSamplerStates();

			std::array<Array<SamplerState>, Graphics::TextureSlotCount> psSamplerStates = MakeDefaultSamplerStates();

			Array<Optional<Rect>> scissorRects		= { none };

			Array<Optional<Rect>> viewports			= { none };

			Array<VertexShader::IDType> vertexShaders;

			Array<PixelShader::IDType> pixelShaders;

			Array<Mat3x2> combinedTransforms		= { Mat3x2::Identity() };

			std::array<Array<Texture::IDType>, Graphics::TextureSlotCount> vsTextures = MakeDefaultTextures();

			std::array<Array<Texture::IDType>, Graphics::TextureSlotCount> psTextures = MakeDefaultTextures();

		} m_buffer;

		struct Current
		{
			D3D11DrawCommand draw;

			Float4 colorMul						= Float4{ 1.0f, 1.0f, 1.0f, 1.0f };
			
			Float3 colorAdd						= Float3{ 0.0f, 0.0f, 0.0f };

			std::array<Float4, 3> quadWarpParameter	= { Float4{ 0.0f, 0.0f, 0.0f, 0.0f }, Float4{ 0.0f, 0.0f, 0.0f, 0.0f }, Float4{ 0.0f, 0.0f, 0.0f, 0.0f } };

			std::array<Float4, 3> patternParameter	= { Float4{ 0.0f, 0.0f, 0.0f, 0.0f }, Float4{ 0.0f, 0.0f, 0.0f, 0.0f }, Float4{ 0.0f, 0.0f, 0.0f, 0.0f } };
		
			BlendState blendState				= BlendState::Default2D;

			RasterizerState rasterizerState		= RasterizerState::Default2D;
			
			std::array<SamplerState, Graphics::TextureSlotCount> vsSamplerStates = MakeDefaultSamplerState();
			
			std::array<SamplerState, Graphics::TextureSlotCount> psSamplerStates = MakeDefaultSamplerState();

			Optional<Rect> scissorRect			= none;

			Optional<Rect> viewport				= none;

			VertexShader::IDType vertexShader	= VertexShader::IDType::Invalid();
			
			PixelShader::IDType pixelShader		= PixelShader::IDType::Invalid();

			Mat3x2 localTransform				= Mat3x2::Identity();
			
			Mat3x2 cameraTransform				= Mat3x2::Identity();
			
			Mat3x2 combinedTransform			= Mat3x2::Identity();

			float maxScaling					= 1.0f;

			std::array<Texture::IDType, Graphics::TextureSlotCount> vsTextures;

			std::array<Texture::IDType, Graphics::TextureSlotCount> psTextures;

		} m_current;

		struct Reserved
		{
			HashMap<Texture::IDType, Texture> textures;

		} m_reserved;

		static std::array<Array<SamplerState>, Graphics::TextureSlotCount> MakeDefaultSamplerStates();

		static std::array<SamplerState, Graphics::TextureSlotCount> MakeDefaultSamplerState();

		static std::array<Array<Texture::IDType>, Graphics::TextureSlotCount> MakeDefaultTextures();
	};
}
