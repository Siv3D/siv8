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

			Array<VertexShader::IDType> vertexShaders;

			Array<PixelShader::IDType> pixelShaders;

		} m_buffer;

		struct Current
		{
			D3D11DrawCommand draw;

			VertexShader::IDType vertexShader	= VertexShader::IDType::Invalid();
			
			PixelShader::IDType pixelShader		= PixelShader::IDType::Invalid();

		} m_current;
	};
}
