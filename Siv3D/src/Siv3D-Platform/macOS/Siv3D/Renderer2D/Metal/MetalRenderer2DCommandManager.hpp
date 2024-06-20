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
# include <Siv3D/BlendState.hpp>
# include <Siv3D/VertexShader.hpp>
# include <Siv3D/PixelShader.hpp>
# include <Siv3D/Renderer/Metal/Metal.hpp>
# include "MetalRenderer2DCommand.hpp"
# include <Siv3D/Renderer2D/BatchStateTracker.hpp>

namespace s3d
{
	class MetalRenderer2DCommandManager
	{
	public:

		MetalRenderer2DCommandManager();

		void reset();

		void flush();

		const Array<MetalRenderer2DCommand>& getCommands() const noexcept;

		void pushDraw(Vertex2D::IndexType indexCount);
		const MetalDrawCommand& getDraw(uint32 index) const noexcept;

		void pushColorMul(const Float4& color);
		const Float4& getColorMul(uint32 index) const;
		const Float4& getCurrentColorMul() const;

		void pushColorAdd(const Float3& color);
		const Float3& getColorAdd(uint32 index) const;
		const Float3& getCurrentColorAdd() const;

		void pushBlendState(const BlendState& state);
		const BlendState& getBlendState(uint32 index) const;
		const BlendState& getCurrentBlendState() const;
	
		void pushEngineVS(VertexShader::IDType id);
		VertexShader::IDType getVS(uint32 index) const;

		void pushEnginePS(PixelShader::IDType id);
		PixelShader::IDType getPS(uint32 index) const;

	private:

		Array<MetalRenderer2DCommand> m_commands;

		BatchStateTracker<MetalRenderer2DCommandType> m_stateTracker;

		struct Buffer
		{
			Array<MetalDrawCommand> draws;

			Array<Float4> colorMuls			= { Float4{ 1.0f, 1.0f, 1.0f, 1.0f } };
			
			Array<Float3> colorAdds			= { Float3{ 0.0f, 0.0f, 0.0f } };
			
			Array<BlendState> blendStates	= { BlendState::Default2D };

			Array<VertexShader::IDType> vertexShaders;

			Array<PixelShader::IDType> pixelShaders;

		} m_buffer;

		struct Current
		{
			MetalDrawCommand draw;

			Float4 colorMul						= Float4{ 1.0f, 1.0f, 1.0f, 1.0f };
			
			Float3 colorAdd						= Float3{ 0.0f, 0.0f, 0.0f };
		
			BlendState blendState				= BlendState::Default2D;

			VertexShader::IDType vertexShader	= VertexShader::IDType::Invalid();
			
			PixelShader::IDType pixelShader		= PixelShader::IDType::Invalid();

		} m_current;
	};
}
