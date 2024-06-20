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

# pragma once
# include <Siv3D/Array.hpp>
# include <Siv3D/Vertex2D.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/BlendState.hpp>
# include <Siv3D/RasterizerState.hpp>
# include <Siv3D/VertexShader.hpp>
# include <Siv3D/PixelShader.hpp>
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

		void pushBlendState(const BlendState& state);
		const BlendState& getBlendState(uint32 index) const;
		const BlendState& getCurrentBlendState() const;

		void pushRasterizerState(const RasterizerState& state);
		const RasterizerState& getRasterizerState(uint32 index) const;
		const RasterizerState& getCurrentRasterizerState() const;

		void pushScissorRect(const Rect& state);
		const Rect& getScissorRect(uint32 index) const;
		const Rect& getCurrentScissorRect() const;

		void pushEngineVS(VertexShader::IDType id);
		VertexShader::IDType getVS(uint32 index) const;

		void pushEnginePS(PixelShader::IDType id);
		PixelShader::IDType getPS(uint32 index) const;

	private:

		Array<D3D11Renderer2DCommand> m_commands;

		BatchStateTracker<D3D11Renderer2DCommandType> m_stateTracker;

		struct Buffer
		{
			Array<D3D11DrawCommand> draws;

			Array<Float4> colorMuls					= { Float4{ 1.0f, 1.0f, 1.0f, 1.0f } };
			
			Array<Float3> colorAdds					= { Float3{ 0.0f, 0.0f, 0.0f } };
			
			Array<BlendState> blendStates			= { BlendState::Default2D };

			Array<RasterizerState> rasterizerStates	= { RasterizerState::Default2D };

			Array<Rect> scissorRects				= { Rect::Empty() };

			Array<VertexShader::IDType> vertexShaders;

			Array<PixelShader::IDType> pixelShaders;

		} m_buffer;

		struct Current
		{
			D3D11DrawCommand draw;

			Float4 colorMul						= Float4{ 1.0f, 1.0f, 1.0f, 1.0f };
			
			Float3 colorAdd						= Float3{ 0.0f, 0.0f, 0.0f };
		
			BlendState blendState				= BlendState::Default2D;

			RasterizerState rasterizerState		= RasterizerState::Default2D;

			Rect scissorRect					= Rect::Empty();

			VertexShader::IDType vertexShader	= VertexShader::IDType::Invalid();
			
			PixelShader::IDType pixelShader		= PixelShader::IDType::Invalid();

		} m_current;
	};
}
