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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/LineStyle.hpp>
# include <Siv3D/Pattern/PatternParameters.hpp>

namespace s3d
{
	class Texture;

	namespace Renderer2DDrawSubmission
	{
		// Call after a successful build with a nonzero index count.
		template <class CommandManager, class EngineShader, class CustomShader>
		inline void SubmitShape(CommandManager& commands, const EngineShader& shaders, const CustomShader& custom, const uint32 indexCount)
		{
			if (not custom.vs)
			{
				commands.pushEngineVS(shaders.vsShape);
			}

			if (not custom.ps)
			{
				commands.pushEnginePS(shaders.psShape);
			}

			commands.pushDraw(indexCount);
		}

		template <class CommandManager, class EngineShader, class CustomShader>
		inline void SubmitPattern(CommandManager& commands, const EngineShader& shaders, const CustomShader& custom, const uint32 indexCount, const PatternParameters& pattern)
		{
			if (not custom.vs)
			{
				commands.pushEngineVS(shaders.vsPattern);
			}

			if (not custom.ps)
			{
				commands.pushEnginePS(shaders.getPatternShader(pattern.type));
			}

			commands.pushPatternParameter(pattern.toFloat4Array());
			commands.pushDraw(indexCount);
		}

		// Resolve an owned texture after shader selection, without extending its live range.
		template <class CommandManager, class EngineShader, class CustomShader, class TextureGetter>
		inline void SubmitTextureFrom(CommandManager& commands, const EngineShader& shaders, const CustomShader& custom, const uint32 indexCount, TextureGetter&& getTexture)
		{
			if (not custom.vs)
			{
				commands.pushEngineVS(shaders.vsShape);
			}

			if (not custom.ps)
			{
				commands.pushEnginePS(shaders.psTexture);
			}

			commands.pushPSTexture(0, getTexture());
			commands.pushDraw(indexCount);
		}

		template <class CommandManager, class EngineShader, class CustomShader>
		inline void SubmitTexture(CommandManager& commands, const EngineShader& shaders, const CustomShader& custom, const uint32 indexCount, const Texture& texture)
		{
			SubmitTextureFrom(commands, shaders, custom, indexCount, [&texture]() -> const Texture& { return texture; });
		}

		template <class CommandManager, class EngineShader, class CustomShader>
		// Keep shader-field address calculations folded into the caller's switch table.
		SIV3D_ALWAYS_INLINE
		inline void SubmitStyledLine(CommandManager& commands, const EngineShader& shaders, const CustomShader& custom, const uint32 indexCount, const LineStyle& style)
		{
			if (not custom.vs)
			{
				commands.pushEngineVS(shaders.vsShape);
			}

			if (not custom.ps)
			{
				switch (style.type)
				{
				case LinePattern::Solid:
					commands.pushEnginePS(shaders.psShape);
					break;
				case LinePattern::Dotted:
					commands.pushEnginePS(shaders.psLineDot);
					break;
				case LinePattern::Dashed:
					commands.pushEnginePS(shaders.psLineDash);
					break;
				case LinePattern::LongDash:
					commands.pushEnginePS(shaders.psLineLongDash);
					break;
				case LinePattern::DashDot:
					commands.pushEnginePS(shaders.psLineDashDot);
					break;
				case LinePattern::RoundDot:
					commands.pushEnginePS(shaders.psLineRoundDot);
					break;
				}
			}

			commands.pushDraw(indexCount);
		}
	}
}
